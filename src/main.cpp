#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#if defined _WIN32
    #include <windows.h>
    #ifdef GetDiskFreeSpace
        #undef GetDiskFreeSpace
    #endif
#elif defined __linux__
    #include <sys/vfs.h>
#endif
#include "Error.h"
#include "FileNode.h"
#include "FileSystem.h"

namespace {

#if defined _WIN32

void GetDiskPartitions(std::vector<std::string> &drives) {
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
                drives.push_back(current);
            }
            current = "";
        } else {
            current.push_back(c);
        }
    }
}

bool GetDiskFreeSpace(const std::string &rootPath,
                      std::int64_t &numberOfBytesFree,
                      std::int64_t &totalSizeInBytes) {
    DWORD sectorsPerCluster;
    DWORD bytesPerSector;
    DWORD freeClusterCount;
    DWORD totalClusterCount;
    if (GetDiskFreeSpaceA(rootPath.c_str(),
                          &sectorsPerCluster,
                          &bytesPerSector,
                          &freeClusterCount,
                          &totalClusterCount)) {
        auto bytesPerClster =
            bytesPerSector * static_cast<std::int64_t>(sectorsPerCluster);
        numberOfBytesFree = bytesPerClster * freeClusterCount;
        totalSizeInBytes = bytesPerClster * totalClusterCount;
        return true;
    }
    return false;
}

#elif defined __linux__

void GetDiskPartitions(std::vector<std::string> &partitions) {
    std::ifstream procPartitionsStream("/proc/partitions");
    std::string line;

    while (std::getline(procPartitionsStream, line)) {
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

        partitions.push_back(devicePath);
    }
}

bool GetDiskFreeSpace(const std::string &rootPath,
                      std::int64_t &numberOfBytesFree,
                      std::int64_t &totalSizeInBytes) {
    return false;
}

#endif

std::string FormatSize(std::int64_t size) {
    static const std::vector<std::pair<std::int64_t, const char *>> units = {
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
    static const std::vector<std::pair<std::int64_t, const char *>> units = {
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

} // anonymous namespace

int main() {
    std::vector<std::string> partitions;
    GetDiskPartitions(partitions);

    std::cout << "Welcome to GarbageFinder!\n\n";

    std::string target;
    for (;;) {
        for (std::size_t i = 0; i < partitions.size(); i++) {
            auto &path = partitions[i];
            std::cout << i + 1 << ". " << path;
            std::int64_t free, total;
            if (GetDiskFreeSpace(path, free, total)) {
                std::cout << " ("
                          << FormatSize(total)
                          << " / "
                          << FormatSize(free) << " free)";
            }
            std::cout << std::endl;
        }
        std::cout << "Choose a volume or directory to analyze: ";
        std::cin >> target;
        std::cout << std::endl;
        if (std::all_of(target.begin(), target.end(), ::isdigit)) {
            int partitionIndex = std::stoi(target, nullptr);
            if (partitionIndex > 0
                && partitionIndex <= static_cast<int>(partitions.size())) {
                target = partitions[partitionIndex - 1];
                break;
            }
        } else {
            gf::Error error;
            auto type = gf::GetFileType(target, error);
            if (error) {
                std::cout << "Error: " + error.Message() << std::endl;
                continue;
            }
            if (type == gf::FileType::Directory) {
                break;
            }
            std::cout << std::endl
                      << "Sorry, this doesn't look like a valid directory path"
                      << std::endl << std::endl;
        }
    }

    std::cout << "Analyzing " << target << " ... ";
    auto startTime = std::chrono::system_clock::now();

    auto tree = gf::BuildFileTree(target, 100);

    auto durationInSeconds = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - startTime);
    std::cout
        << "Done (" << durationInSeconds.count() << " seconds)"
        << std::endl
        << std::endl
        << "Total size: " << FormatSize(tree->Size())
        << std::endl;

    return 0;
}
