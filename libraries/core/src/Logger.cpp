#include "Logger.h"

#include <Windows.h>


namespace Core
{
    static constexpr size_t k_BufferSize = 1024;


    Logger::Logger(const char* fileName)
    {
        fopen_s(&m_LogFile, fileName, "a");
    }

    Logger::~Logger()
    {
        fclose(m_LogFile);
    }

    void Logger::Info(const char* format, ...) const
    {
        char buffer[k_BufferSize] = {};
        {
            va_list args = nullptr;
            va_start(args, format);
            vsprintf_s(buffer, format, args);
            va_end(args);
        }
        
        Log("INFO", buffer);
    }

    void Logger::Warning(const char* format, ...) const
    {
        char buffer[k_BufferSize] = {};
        {
            va_list args = nullptr;
            va_start(args, format);
            vsprintf_s(buffer, format, args);
            va_end(args);
        }
        
        Log("WARNING", buffer);
    }

    void Logger::Error(const char* format, ...) const
    {
        char buffer[k_BufferSize] = {};
        {
            va_list args = nullptr;
            va_start(args, format);
            vsprintf_s(buffer, format, args);
            va_end(args);
        }
        
        Log("ERROR", buffer);
    }

    void Logger::Log(const char* logLevel, const char* message) const
    {
        SYSTEMTIME systemTime = {};
        GetLocalTime(&systemTime);

        fprintf_s(
            m_LogFile,
            "[%04d-%02d-%02d %02d:%02d:%02d.%04d] %8s -- : %s\n",
            systemTime.wYear,
            systemTime.wMonth,
            systemTime.wDay,
            systemTime.wHour,
            systemTime.wMinute,
            systemTime.wSecond,
            systemTime.wMilliseconds,
            logLevel,
            message
        );

        fflush(m_LogFile);
    }
}