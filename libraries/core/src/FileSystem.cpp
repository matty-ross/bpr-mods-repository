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

    void FileSystem::ReadFileContent(const char* path, void* buffer, size_t size)
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

        DWORD bytesRead = 0;
        BOOL result = ReadFile(fileHandle, buffer, size, &bytesRead, nullptr);
        
        CloseHandle(fileHandle);

        if (result == FALSE || bytesRead != size)
        {
            throw WindowsException("Failed to read file content");
        }
    }

    void FileSystem::WriteFileContent(const char* path, const void* buffer, size_t size)
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
        BOOL result = WriteFile(fileHandle, buffer, size, &bytesWritten, nullptr);
        
        CloseHandle(fileHandle);

        if (result == FALSE || bytesWritten != size)
        {
            throw WindowsException("Failed to write file content");
        }
    }
}
