#include "Logger.hpp"

#include <cstdio>
#include <cstdarg>
#include <Windows.h>


namespace Core
{
    static constexpr size_t k_MessageMaxSize = 1024;


    Logger::Logger(const char* name)
        :
        m_Name(name)
    {
    }

    void Logger::Info(const char* format, ...) const
    {
        char message[k_MessageMaxSize] = {};
        
        va_list args;
        va_start(args, format);
        vsprintf_s(message, format, args);
        va_end(args);
        
        Log("\x1B[1;32mINFO\x1B[0m", message);
    }

    void Logger::Warning(const char* format, ...) const
    {
        char message[k_MessageMaxSize] = {};
        
        va_list args;
        va_start(args, format);
        vsprintf_s(message, format, args);
        va_end(args);
        
        Log("\x1B[1;33mWARNING\x1B[0m", message);
    }

    void Logger::Error(const char* format, ...) const
    {
        char message[k_MessageMaxSize] = {};
        
        va_list args;
        va_start(args, format);
        vsprintf_s(message, format, args);
        va_end(args);
        
        Log("\x1B[1;31mERROR\x1B[0m", message);
    }

    void Logger::Log(const char* level, const char* message) const
    {
        SYSTEMTIME localTime = {};
        GetLocalTime(&localTime);

        char buffer[80 + k_MessageMaxSize] = {};    
        int length = sprintf_s(
            buffer,
            "[%04d-%02d-%02d %02d:%02d:%02d.%03d]  %-20s  %-20s  :  %s\n",
            localTime.wYear,
            localTime.wMonth,
            localTime.wDay,
            localTime.wHour,
            localTime.wMinute,
            localTime.wSecond,
            localTime.wMilliseconds,
            level,
            m_Name,
            message
        );

        HANDLE consoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        WriteConsoleA(consoleOutputHandle, buffer, length, nullptr, nullptr);
    }
}
