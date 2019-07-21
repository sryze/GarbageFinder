#include <limits>
#include <string>

#ifndef GF_FILE_TREE_H
#define GF_FILE_TREE_H

namespace gf {

class FileNode;

std::shared_ptr<FileNode> BuildFileTree(
    const std::wstring &rootPath,
    int maxDepth = std::numeric_limits<int>::max());

} // namespace gf

#endif
