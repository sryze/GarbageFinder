#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "FileInfo.h"

#ifndef GF_FILE_NODE_H
#define GF_FILE_NODE_H

namespace gf {

class FileNode
{
public:
    FileNode(FileType type, std::string name, FileSize size);

    std::string Name() const;
    FileType Type() const;
    FileSize Size() const;
    void SetSize(FileSize size);

    bool AddChildNode(std::shared_ptr<FileNode> node);
    void ForEachChildNode(
        const std::function<bool(std::shared_ptr<FileNode>)> &func) const;

private:
    class Hasher
    {
    public:
        std::size_t operator()(const std::shared_ptr<FileNode> &node) const
        {
            return std::hash<std::string>()(node->name_);
        }
    };

    class Comparator
    {
    public:
        bool operator()(const std::shared_ptr<FileNode> &node1,
                        const std::shared_ptr<FileNode> &node2) const
        {
            return node1->name_ < node2->name_;
        }
    };

    std::string name_;
    FileType type_;
    FileSize size_;
    std::set<
        std::shared_ptr<FileNode>,
        Comparator> childNodes_;
};

} // namespace gf

#endif
