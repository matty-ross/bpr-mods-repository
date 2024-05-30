#include "WindowsException.hpp"

#include <cstdio>
#include <Windows.h>


namespace Core
{
    WindowsException::WindowsException(const char* message)
    {
        DWORD lastError = GetLastError();
        sprintf_s(m_Buffer, "%s. Last error: %lu.", message, lastError);
    }
    
    const char* WindowsException::what() const noexcept
    {
        return m_Buffer;
    }
}
