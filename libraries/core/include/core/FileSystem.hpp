#pragma once


namespace Core
{
    class FileSystem
    {
    public:
        static void CreateFolder(const char* path);
        static void ReadFileContent(const char* path, void* buffer, size_t size);
        static void WriteFileContent(const char* path, const void* buffer, size_t size);
    };
}
