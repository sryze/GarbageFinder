#include <cstdint>
#include <limits>
#include <string>

#ifndef GF_FILE_SYSTEM_H
#define GF_FILE_SYSTEM_H

namespace gf {

class Error;
class FileNode;

void GetMountedVolumes(std::vector<std::string>& volumes);
bool GetVolumeFreeSpace(
    const std::string &path, FileSize &free, FileSize &total);

FileInfo GetFileInfo(const std::string &path, Error &error);
FileType GetFileType(const std::string &path, Error &error);

std::shared_ptr<FileNode> ReadFileTree(const std::string &path, int maxDepth);

std::string FormatSize(FileSize size);
std::string FormtSizeMetric(FileSize size);

} // namespace gf

#endif
