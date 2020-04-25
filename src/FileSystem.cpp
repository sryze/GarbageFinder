#include <string_view>
#ifdef _WIN32
    #include <windows.h>
    #define stat _stat
    #define S_ISDIR(mode) ((_S_IFDIR & mode) != 0)
    #define S_ISLNK(mode) ((_S_IFLNK & mode) != 0)
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <unistd.h>
#endif
#include "Error.h"
#include "FileNode.h"
#include "FileSystem.h"

namespace gf {

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

void BuildFileTreeInternal(std::shared_ptr<FileNode> root,
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
            BuildFileTreeInternal(node, path, maxDepth - 1);
        }
        totalSize += node->Size();
        return true;
    });

    root->SetSize(totalSize);
}

} // anonymous namespace

std::shared_ptr<FileNode> BuildFileTree(const std::string &rootPath,
                                        int maxDepth) {
    auto rootNode =
        std::make_shared<FileNode>(FileType::Directory, rootPath, 0);
    BuildFileTreeInternal(rootNode, rootPath, maxDepth);
    return rootNode;
}

} // namespace gf
