#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include "Error.h"
#include "FileNode.h"
#include "FileSystem.h"

using namespace gf;

static const int MAX_TREE_DEPTH = 100;

int main() {
    std::vector<std::string> volumes;
    GetMountedVolumes(volumes);

    std::cout << "Welcome to Garbage Finder!" << std::endl;

    std::string target;
    for (;;) {
        if (!volumes.empty()) {
            std::cout << std::endl;
        }
        for (std::size_t i = 0; i < volumes.size(); i++) {
            auto &path = volumes[i];
            std::cout << i + 1 << ". " << path;
            FileSize free, total;
            if (GetVolumeFreeSpace(path, free, total)) {
                std::cout << " ("
                          << FormatSize(total)
                          << " / "
                          << FormatSize(free) << " free)";
            }
            std::cout << std::endl;
        }
        if (volumes.empty()) {
            std::cout << std::endl << "Directory to analyze: ";
        } else {
            std::cout << std::endl
                << "Select a volume [" << 1 << "-" << volumes.size() << "] "
                << "or directory to analyze: ";
        }
        std::cin >> target;
        std::cout << std::endl;
        if (std::all_of(target.begin(), target.end(), ::isdigit)) {
            auto volumeIndex =
                static_cast<std::size_t>(std::stoi(target, nullptr));
            if (volumeIndex > 0 && volumeIndex <= volumes.size()) {
                target = volumes[volumeIndex - 1];
                break;
            }
        } else {
            Error error;
            auto type = GetFileType(target, error);
            if (error) {
                std::cout << "Error "
                          << error.Code() << ": " + error.Message()
                          << std::endl;
                continue;
            }
            if (type == FileType::Directory) {
                break;
            }
            std::cout << "Sorry, this doesn't look like a valid directory path"
                      << std::endl;
        }
    }

    std::cout << "Analyzing " << target << " ... ";
    auto startTime = std::chrono::system_clock::now();

    auto tree = ReadFileTree(target, MAX_TREE_DEPTH);

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
