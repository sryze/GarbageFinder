#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#include "FileNode.h"
#include "FileTree.h"

namespace {

void GetLogicalDrives(std::vector<std::wstring> &drives) {
	std::vector<wchar_t> buffer;
	buffer.resize(256);
	
	for (;;) {
		auto result = GetLogicalDriveStringsW(
			static_cast<DWORD>(buffer.capacity() / sizeof(wchar_t)),
			buffer.data());
		if (result <= buffer.capacity()) {
			break;
		}
		buffer.resize(buffer.capacity() * 2);
	}

	std::wstring current;

	for (auto &c : buffer) {
		if (c == '\0') {
			if (!current.empty()) {
				drives.push_back(current);
			}
			current = L"";
		} else {
			current.push_back(c);
		}
	}
}

std::string FormtSize(std::int64_t size) {
    static std::vector<std::pair<std::int64_t, const char *>> units = {
        {1LL << 40, "TiB"},
        {1LL << 30, "GiB"},
        {1LL << 20, "MiB"},
        {1LL << 10, "KiB"}
    };
    for (auto [unitSize, suffix] : units) {
        if (size >= unitSize) {
            auto countX10 = size * 10 / unitSize;
            auto stream = std::ostringstream{};
            stream << countX10 / 10 << "." << countX10 % 10 << " " << suffix;
            return stream.str();
        }
    }
    return std::to_string(size) + " bytes";
}

std::string FormtSizeMetric(std::int64_t size) {
    static std::vector<std::pair<std::int64_t, const char *>> units = {
        {1'000'000'000'000LL, "TB"},
        {1'000'000'000LL, "GB"},
        {1'000'000LL, "MB"},
        {1'000LL, "KB"}
    };
    for (auto [unitSize, suffix] : units) {
        if (size >= unitSize) {
            auto countX10 = size * 10 / unitSize;
            auto stream = std::ostringstream{};
            stream << countX10 / 10 << "." << countX10 % 10 << " " << suffix;
            return stream.str();
        }
    }
    return std::to_string(size) + " bytes";
}

} // anonymous namespace

int main() {
	std::vector<std::wstring> drives;
	GetLogicalDrives(drives);

	int driveNumber = 0;
	for (;;) {
		for (std::size_t i = 0; i < drives.size(); i++) {
			std::wcout << i + 1 << ". " << drives[i] << std::endl;
		}
		std::cout << "Choose a logical drive to analyze: ";
		std::cin >> driveNumber;
		std::cout << std::endl;
		if (driveNumber > 0 && driveNumber <= drives.size()) {
			break;
		}
	}

    std::wstring selectedDrive = drives[driveNumber - 1];
	std::wcout << L"Analyzing " << selectedDrive << " ... ";

    auto startTime = std::chrono::system_clock::now();
	auto tree = gf::BuildFileTree(selectedDrive);

    auto durationInSeconds = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - startTime);
    std::cout
        << "Done (" << durationInSeconds.count() << " seconds)"
        << std::endl
        << std::endl
        << "Total size: " << FormtSizeMetric(tree->Size())
                          << " (" << FormtSize(tree->Size()) << ")"
        << std::endl;

	return 0;
}
