#pragma once


#include <string>

#include <Windows.h>


namespace Core
{
    class File
    {
    public:
        File(const std::string& filePath, uint32_t desiredAccess, uint32_t shareMode, uint32_t creationDisposition);
        ~File();

        File(const File&) = delete;
        File(File&&) = delete;
        File& operator =(const File&) = delete;
        File& operator =(File&&) = delete;

    public:
        std::string Read() const;
        void Write(const std::string& content) const;

    private:
        HANDLE m_FileHandle = INVALID_HANDLE_VALUE;
    };
}