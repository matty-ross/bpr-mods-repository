#pragma once


#include <string>

#include <Windows.h>


namespace Core
{
    class File
    {
    public:
        enum class Operation
        {
            Read,
            Write,
        };

    public:
        File(const std::string& filePath, Operation fileOperation);
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
