#include "File.hpp"

#include "WindowsException.hpp"


namespace Core
{
    File::File(const std::string& filePath, Mode mode)
    {
        DWORD desiredAccess = 0;
        DWORD creationDisposition = 0;
        
        switch (mode)
        {
        case Mode::Read:
            desiredAccess = GENERIC_READ;
            creationDisposition = OPEN_EXISTING;
            break;

        case Mode::Write:
            desiredAccess = GENERIC_WRITE;
            creationDisposition = CREATE_ALWAYS;
            break;
        }
        
        m_FileHandle = CreateFileA(filePath.c_str(), desiredAccess, FILE_SHARE_READ, nullptr, creationDisposition, 0, NULL);
        if (m_FileHandle == INVALID_HANDLE_VALUE)
        {
            throw WindowsException("Cannot open/create file");
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
        std::string content(size, '\0');

        DWORD bytesRead = 0;
        if (ReadFile(m_FileHandle, content.data(), content.size(), &bytesRead, nullptr) == FALSE)
        {
            throw WindowsException("Cannot read file");
        }

        return content;
    }
    
    void File::Write(const std::string& content) const
    {
        DWORD bytesWritten = 0;
        if (WriteFile(m_FileHandle, content.data(), content.size(), &bytesWritten, nullptr) == FALSE)
        {
            throw WindowsException("Cannot write file");
        }
    }
}
