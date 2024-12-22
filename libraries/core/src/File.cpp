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

    size_t File::GetSize() const
    {
        return GetFileSize(m_FileHandle, nullptr);
    }

    std::string File::ReadText() const
    {
        std::string content(GetSize(), '\0');
        Read(content.data(), content.size());
        return content;
    }

    std::vector<BYTE> File::ReadBinary() const
    {
        std::vector<BYTE> content(GetSize(), 0x00);
        Read(content.data(), content.size());
        return content;
    }

    void File::WriteText(const std::string& content) const
    {
        Write(content.data(), content.size());
    }

    void File::WriteBinary(const std::vector<BYTE>& content) const
    {
        Write(content.data(), content.size());
    }
    
    void File::Read(void* buffer, size_t size) const
    {
        DWORD bytesRead = 0;
        if (ReadFile(m_FileHandle, buffer, size, &bytesRead, nullptr) == FALSE)
        {
            throw WindowsException("Cannot read file");
        }
    }
    
    void File::Write(const void* buffer, size_t size) const
    {
        DWORD bytesWritten = 0;
        if (WriteFile(m_FileHandle, buffer, size, &bytesWritten, nullptr) == FALSE)
        {
            throw WindowsException("Cannot write file");
        }
    }
}
