#include <limits>
#include <string>

#ifndef GF_FILE_SYSTEM_H
#define GF_FILE_SYSTEM_H

namespace gf {

enum class FileType;

class Error;
class FileNode;

FileType GetFileType(const std::string &path, Error &error);

std::shared_ptr<FileNode> BuildFileTree(const std::string &rootPath,
                                        int maxDepth);

} // namespace gf

#endif
