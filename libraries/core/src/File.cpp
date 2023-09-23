#include "File.h"

#include <stdexcept>


namespace Core
{
    File::File(const char* filePath, uint32_t desiredAccess, uint32_t shareMode, uint32_t creationDisposition)
    {
        m_FileHandle = CreateFileA(filePath, desiredAccess, shareMode, nullptr, creationDisposition, 0, nullptr);
        if (m_FileHandle == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("Cannot open/create file.");
        }
    }

    File::~File()
    {
        if (m_FileHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_FileHandle);
        }
    }
    
    std::string File::Read() const
    {
        size_t size = GetFileSize(m_FileHandle, nullptr);
        std::string content(size, ' ');

        DWORD bytesRead = 0;
        if (ReadFile(m_FileHandle, content.data(), content.size(), &bytesRead, nullptr) == FALSE)
        {
            throw std::runtime_error("Cannot read file.");
        }

        return content;
    }
    
    void File::Write(const std::string& content) const
    {
        DWORD bytesWritten = 0;
        if (WriteFile(m_FileHandle, content.data(), content.size(), &bytesWritten, nullptr) == FALSE)
        {
            throw std::runtime_error("Cannot write file.");
        }
    }
}