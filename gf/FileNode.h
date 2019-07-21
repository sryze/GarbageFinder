#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>

#ifndef GF_FILE_NODE_H
#define GF_FILE_NODE_H

namespace gf {

enum class FileType
{
    File,
    Directory,
    Link
};

class FileNode 
{
public:
	FileNode(FileType type, std::wstring name, std::int64_t size);

    FileType Type() const;
    std::wstring Name() const;
    std::int64_t Size() const;
    void SetSize(std::int64_t size);

	bool AddChildNode(std::shared_ptr<FileNode> node);
    void ForEachChildNode(
        const std::function<bool(std::shared_ptr<FileNode>)> &func) const;

private:
    class Hasher
    {
    public:
        std::size_t operator()(const std::shared_ptr<FileNode> &node) const
        {
            return std::hash<std::wstring>()(node->name_);
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

    FileType type_;
	std::wstring name_;
    std::int64_t size_;
	std::set<
        std::shared_ptr<FileNode>,
        Comparator> childNodes_;
};

} // namespace gf

#endif
