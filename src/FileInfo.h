#ifndef GF_FILE_INFO_H
#define GF_FILE_INFO_H

typedef std::int64_t FileSize;

enum class FileType {
    File,
    Directory,
    Link
};

struct FileInfo {
    FileType type;
    FileSize size;
};

#endif
