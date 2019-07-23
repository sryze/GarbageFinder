#include <limits>
#include <string>

#ifndef GF_FILE_TREE_H
#define GF_FILE_TREE_H

namespace gf {

class FileNode;

constexpr auto DefaultMaxFileTreeDepth = 128;

std::shared_ptr<FileNode> BuildFileTree(
    const std::wstring &rootPath, int maxDepth = DefaultMaxFileTreeDepth);

} // namespace gf

#endif
