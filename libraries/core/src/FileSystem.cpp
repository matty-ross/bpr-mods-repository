#include "FileSystem.hpp"

#include <Windows.h>
#include <ShlObj.h>

#include "WindowsException.hpp"


namespace Core
{
    void FileSystem::CreateFolder(const char* path)
    {
        char expandedPath[MAX_PATH] = {};
        if (ExpandEnvironmentStringsA(path, expandedPath, sizeof(expandedPath)) == 0)
        {
            throw WindowsException("Failed to expand file path");
        }

        int result = SHCreateDirectoryExA(NULL, expandedPath, nullptr);
        if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS)
        {
            SetLastError(result);
            throw WindowsException("Failed to create directory");
        }
    }

    void FileSystem::ReadFileContent(const char* path, FileContent& content)
    {
        char expandedPath[MAX_PATH] = {};
        if (ExpandEnvironmentStringsA(path, expandedPath, sizeof(expandedPath)) == 0)
        {
            throw WindowsException("Failed to expand file path");
        }

        HANDLE fileHandle = CreateFileA(expandedPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, NULL);
        if (fileHandle == INVALID_HANDLE_VALUE)
        {
            throw WindowsException("Failed to open file");
        }

        DWORD fileSize = GetFileSize(fileHandle, nullptr);
        content.m_Buffer = new BYTE[fileSize];
        content.m_Size = fileSize;
        content.m_OwnsBuffer = true;

        DWORD bytesRead = 0;
        ReadFile(fileHandle, content.m_Buffer, content.m_Size, &bytesRead, nullptr);
        
        CloseHandle(fileHandle);
    }

    void FileSystem::WriteFileContent(const char* path, const FileContent& content)
    {
        char expandedPath[MAX_PATH] = {};
        if (ExpandEnvironmentStringsA(path, expandedPath, sizeof(expandedPath)) == 0)
        {
            throw WindowsException("Failed to expand file path");
        }

        HANDLE fileHandle = CreateFileA(expandedPath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, NULL);
        if (fileHandle == INVALID_HANDLE_VALUE)
        {
            throw WindowsException("Failed to create file");
        }

        DWORD bytesWritten = 0;
        WriteFile(fileHandle, content.m_Buffer, content.m_Size, &bytesWritten, nullptr);
        
        CloseHandle(fileHandle);
    }
}
