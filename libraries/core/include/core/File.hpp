#pragma once


#include <string>
#include <vector>
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
        size_t GetSize() const;

        std::string ReadText() const;
        std::vector<BYTE> ReadBinary() const;
        void WriteText(const std::string& content) const;
        void WriteBinary(const std::vector<BYTE>& content) const;

    private:
        void Read(void* buffer, size_t size) const;
        void Write(const void* buffer, size_t size) const;

    private:
        HANDLE m_FileHandle = INVALID_HANDLE_VALUE;
    };
}
