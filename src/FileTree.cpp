#include <string_view>
#include <windows.h>
#include "FileNode.h"
#include "FileTree.h"

namespace gf {
namespace {

void BuildFileTreeInternal(std::shared_ptr<FileNode> root,
                           const std::wstring &rootPath,
                           int maxDepth)
{
    if (maxDepth == 0) {
        return;
    }

    WIN32_FIND_DATAW findData;
    auto pattern = rootPath + L"\\*";
    auto findHandle = FindFirstFileW(pattern.c_str(), &findData);

    if (findHandle == INVALID_HANDLE_VALUE) {
        return;
    }

    bool foundNext = false;
    do {
        std::wstring_view name(findData.cFileName);
        if (name == L"." || name == L"..") {
            foundNext = FindNextFileW(findHandle, &findData);
            continue;
        }

        std::int64_t size =
            (static_cast<std::int64_t>(findData.nFileSizeHigh) << 32)
            | findData.nFileSizeLow;
        FileType type;
        auto attributes = findData.dwFileAttributes;
        if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0) {
            type = FileType::Link;
        } else if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            type = FileType::Directory;
        } else {
            type = FileType::File;
        }
        auto file = std::make_shared<FileNode>(type, std::wstring(name), size);
        root->AddChildNode(file);

        foundNext = FindNextFileW(findHandle, &findData);
    } while (foundNext);

    FindClose(findHandle);

    std::int64_t totalSize = 0;
    root->ForEachChildNode([
        &totalSize,
        &root,
        &rootPath,
        &maxDepth
    ](std::shared_ptr<FileNode> node) {
        if (node->Type() == FileType::Directory) {
            auto path = (rootPath + L"\\") + node->Name();
            BuildFileTreeInternal(node, path, maxDepth - 1);
        }
        totalSize += node->Size();
        return true;
    });

    root->SetSize(totalSize);
}

} // anonymous namespace

std::shared_ptr<FileNode> BuildFileTree(const std::wstring &rootPath,
                                        int maxDepth)
{
    auto rootNode =
        std::make_shared<FileNode>(FileType::Directory, rootPath, 0);
    BuildFileTreeInternal(
        rootNode,
        rootPath,
        maxDepth);
    return rootNode;
}

} // namespace gf
