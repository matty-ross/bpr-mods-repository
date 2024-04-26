#pragma once


namespace Core
{
    class Logger
    {
    public:
        Logger(const char* name);

        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator =(const Logger&) = delete;
        Logger& operator =(Logger&&) = delete;

    public:
        void Info(const char* format, ...) const;
        void Warning(const char* format, ...) const;
        void Error(const char* format, ...) const;

    private:
        void Log(const char* logLevel, const char* message) const;

    private:
        const char* m_Name = nullptr;
    };
}
