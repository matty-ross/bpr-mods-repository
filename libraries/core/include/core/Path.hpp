#pragma once


#include <Windows.h>


namespace Core
{
    class Path
    {
    public:
        Path(const char* path);

    public:
        const char* GetPath() const;

        Path& Append(const char* path);
        void Create() const;

    private:
        char m_Path[MAX_PATH] = {};
    };
}
