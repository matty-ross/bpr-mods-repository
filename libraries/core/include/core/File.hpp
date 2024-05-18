#pragma once


#include <string>
#include <Windows.h>


namespace Core
{
    class File
    {
    public:
        enum class Mode
        {
            Read,
            Write,
        };

    public:
        File(const std::string& filePath, Mode mode);
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
