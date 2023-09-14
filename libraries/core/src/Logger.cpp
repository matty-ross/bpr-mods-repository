#include "Logger.h"

#include <cstdio>
#include <cstdarg>

#include <Windows.h>


namespace Core
{
    static constexpr size_t k_MessageSize = 1024;


    Logger::Logger(const char* name)
        :
        m_Name(name)
    {
    }

    void Logger::Info(const char* format, ...) const
    {
        char message[k_MessageSize] = {};
        {
            va_list args = nullptr;
            va_start(args, format);
            vsprintf_s(message, format, args);
            va_end(args);
        }
        
        Log("INFO", message);
    }

    void Logger::Warning(const char* format, ...) const
    {
        char message[k_MessageSize] = {};
        {
            va_list args = nullptr;
            va_start(args, format);
            vsprintf_s(message, format, args);
            va_end(args);
        }
        
        Log("WARNING", message);
    }

    void Logger::Error(const char* format, ...) const
    {
        char message[k_MessageSize] = {};
        {
            va_list args = nullptr;
            va_start(args, format);
            vsprintf_s(message, format, args);
            va_end(args);
        }
        
        Log("ERROR", message);
    }

    void Logger::Log(const char* logLevel, const char* message) const
    {
        SYSTEMTIME systemTime = {};
        GetLocalTime(&systemTime);

        printf_s(
            "[%04d-%02d-%02d %02d:%02d:%02d.%04d]  %-20s %8s -- : %s\n",
            systemTime.wYear,
            systemTime.wMonth,
            systemTime.wDay,
            systemTime.wHour,
            systemTime.wMinute,
            systemTime.wSecond,
            systemTime.wMilliseconds,
            m_Name,
            logLevel,
            message
        );
    }
}