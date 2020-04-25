#include <limits>
#include <string>

#ifndef GF_FILE_SYSTEM_H
#define GF_FILE_SYSTEM_H

namespace gf {

enum class FileType;

class Error;
class FileNode;

void GetMountedVolumes(std::vector<std::string>& volumes);
bool GetVolumeFreeSpace(
    const std::string &path, std::int64_t &free, std::int64_t &total);

FileType GetFileType(const std::string &path, Error &error);

std::shared_ptr<FileNode> ReadFileTree(const std::string &path, int maxDepth);

std::string FormatSize(std::int64_t size);
std::string FormtSizeMetric(std::int64_t size);

} // namespace gf

#endif
