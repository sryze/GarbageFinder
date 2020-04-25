#include "FileNode.h"

namespace gf {

FileNode::FileNode(FileType type, std::string name, std::int64_t size):
    type_(type), name_(name), size_(size)
{
}

FileType FileNode::Type() const {
    return type_;
}

std::string FileNode::Name() const {
    return name_;
}

std::int64_t FileNode::Size() const {
    return size_;
}

void FileNode::SetSize(std::int64_t size) {
    size_ = size;
}

bool FileNode::AddChildNode(std::shared_ptr<FileNode> node) {
    return childNodes_.insert(node).second;
}

void FileNode::ForEachChildNode(
    const std::function<bool(std::shared_ptr<FileNode>)> &func) const
{
    for (auto &node : childNodes_) {
        if (!func(node)) {
            break;
        }
    }
}

} // namespace gf
