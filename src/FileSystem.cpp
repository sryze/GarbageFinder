#ifdef _WIN32
    #include <windows.h>
    #define stat _stat
    #define S_ISDIR(mode) ((_S_IFDIR & mode) != 0)
    #define S_ISLNK(mode) ((_S_IFLNK & mode) != 0)
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #ifdef __linux__
        #include <sys/vfs.h>
    #endif
    #include <dirent.h>
    #include <unistd.h>
#endif
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "Error.h"
#include "FileNode.h"
#include "FileSystem.h"

namespace gf {

#if defined _WIN32

void GetMountedVolumes(std::vector<std::string> &volumes) {
    std::vector<char> buffer;
    buffer.resize(256);

    for (;;) {
        auto result = GetLogicalDriveStringsA(
            static_cast<DWORD>(buffer.capacity() / sizeof(wchar_t)),
            buffer.data());
        if (result <= buffer.capacity()) {
            break;
        }
        buffer.resize(buffer.capacity() * 2);
    }

    std::string current;

    for (auto &c : buffer) {
        if (c == '\0') {
            if (!current.empty()) {
                volumes.push_back(current);
            }
            current = "";
        } else {
            current.push_back(c);
        }
    }
}

bool GetVolumeFreeSpace(const std::string &path,
                        std::int64_t &free,
                        std::int64_t &total) {
    DWORD sectorsPerCluster;
    DWORD bytesPerSector;
    DWORD freeClusterCount;
    DWORD totalClusterCount;
    if (GetDiskFreeSpaceA(path.c_str(),
                          &sectorsPerCluster,
                          &bytesPerSector,
                          &freeClusterCount,
                          &totalClusterCount)) {
        auto bytesPerClster =
            bytesPerSector * static_cast<std::int64_t>(sectorsPerCluster);
        free = bytesPerClster * freeClusterCount;
        total = bytesPerClster * totalClusterCount;
        return true;
    }
    return false;
}

#elif defined __linux__

void GetMountedVolumes(std::vector<std::string> &volumes) {
    std::ifstream procvolumesStream("/proc/volumes");
    std::string line;

    while (std::getline(procvolumesStream, line)) {
        if (line.find("#blocks") != std::string::npos) {
            continue;
        }
        if (line.length() == 0) {
            continue;
        }

        auto whitespace = " \t";
        auto majorPos = line.find_first_not_of(whitespace);
        if (majorPos == std::string::npos) {
            continue;
        }
        auto minorPos = line.find_first_not_of(
            whitespace, line.find_first_of(whitespace, majorPos));
        if (minorPos == std::string::npos) {
            continue;
        }
        auto numBlocksPos = line.find_first_not_of(
            whitespace, line.find_first_of(whitespace, minorPos));
        if (numBlocksPos == std::string::npos) {
            continue;
        }
        auto namePos = line.find_first_not_of(
            whitespace, line.find_first_of(whitespace, numBlocksPos));
        if (namePos == std::string::npos) {
            continue;
        }

        auto deviceName = line.substr(namePos);
        auto devicePath = "/dev/" + deviceName;

        struct statfs fsInfo;
        if (statfs(devicePath.c_str(), &fsInfo) < 0) {
            continue;
        }

        volumes.push_back(devicePath);
    }
}

bool GetVolumeFreeSpace(const std::string &path,
                        std::int64_t &free,
                        std::int64_t &total) {
    return false;
}

#endif

FileType GetFileType(const std::string &path, Error &error) {
    struct stat fileInfo;
    if (stat(path.c_str(), &fileInfo) < 0) {
        error = Error(ErrorDomain::System, errno);
        return FileType::File;
    }
    if (S_ISDIR(fileInfo.st_mode)) {
        return FileType::Directory;
    }
#ifdef _WIN32
    DWORD attributes = GetFileAttributesA(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        error = Error(ErrorDomain::Win32, GetLastError());
        return FileType::File;
    }
#else
    if (S_ISLNK(fileInfo.st_mode)) {
        return FileType::Link;
    }
#endif
    return FileType::File;
}

namespace {

#ifdef _WIN32

void WalkDirectory(const std::string &path,
                   const std::function<void(
                       const std::string &name,
                       FileType type,
                       std::int64_t size)>
                   callback) {
    WIN32_FIND_DATAA findData;
    auto pattern = path + "\\*";
    auto findHandle = FindFirstFileA(pattern.c_str(), &findData);

    if (findHandle == INVALID_HANDLE_VALUE) {
        return;
    }

    bool foundNext = false;
    do {
        std::string name(findData.cFileName);
        if (name == "." || name == "..") {
            foundNext = FindNextFileA(findHandle, &findData);
            continue;
        }

        std::int64_t size =
            (static_cast<std::int64_t>(findData.nFileSizeHigh) << 32)
            | findData.nFileSizeLow;
        FileType type;
        auto attributes = findData.dwFileAttributes;
        if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0) {
            type = FileType::Link;
        } else if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            type = FileType::Directory;
        } else {
            type = FileType::File;
        }
        callback(name, type, size);

        foundNext = FindNextFileA(findHandle, &findData);
    } while (foundNext);

    FindClose(findHandle);
}

#else

void WalkDirectory(const std::string &path,
                   const std::function<void(
                       const std::string &name,
                       FileType type,
                       std::int64_t size)>
                   callback) {

}

#endif // _WIN32

void ReadFileTreeInternal(std::shared_ptr<FileNode> root,
                          const std::string &rootPath,
                          int maxDepth) {
    if (maxDepth == 0) {
        return;
    }

    WalkDirectory(rootPath,
                  [root](const std::string &name,
                         FileType type,
                         std::int64_t size) {
        auto file = std::make_shared<FileNode>(type, std::string(name), size);
        root->AddChildNode(file);
    });

    std::int64_t totalSize = 0;
    root->ForEachChildNode([
        &totalSize,
        &root,
        &rootPath,
        &maxDepth
    ](std::shared_ptr<FileNode> node) {
        if (node->Type() == FileType::Directory) {
            auto path = (rootPath + "\\") + node->Name();
            ReadFileTreeInternal(node, path, maxDepth - 1);
        }
        totalSize += node->Size();
        return true;
    });

    root->SetSize(totalSize);
}

} // anonymous namespace

std::shared_ptr<FileNode> ReadFileTree(const std::string &path, int maxDepth) {
    auto rootNode =
        std::make_shared<FileNode>(FileType::Directory, path, 0);
    ReadFileTreeInternal(rootNode, path, maxDepth);
    return rootNode;
}

std::string FormatSize(std::int64_t size) {
    static const std::vector<std::pair<std::int64_t, const char*>> units = {
        {1LL << 40, "TB"},
        {1LL << 30, "GB"},
        {1LL << 20, "MB"},
        {1LL << 10, "KB"}
    };
    for (auto p : units) {
        auto unitSize = p.first;
        if (size >= unitSize) {
            auto countX10 = size * 10 / unitSize;
            auto stream = std::ostringstream{};
            auto suffix = p.second;
            stream << countX10 / 10 << "." << countX10 % 10 << " " << suffix;
            return stream.str();
        }
    }
    return std::to_string(size) + " bytes";
}

std::string FormtSizeMetric(std::int64_t size) {
    static const std::vector<std::pair<std::int64_t, const char*>> units = {
        {1000000000000LL, "TB"},
        {1000000000LL, "GB"},
        {1000000LL, "MB"},
        {1000LL, "KB"}
    };
    for (auto p : units) {
        auto unitSize = p.first;
        if (size >= unitSize) {
            auto countX10 = size * 10 / unitSize;
            auto stream = std::ostringstream{};
            auto suffix = p.second;
            stream << countX10 / 10 << "." << countX10 % 10 << " " << suffix;
            return stream.str();
        }
    }
    return std::to_string(size) + " bytes";
}

} // namespace gf
