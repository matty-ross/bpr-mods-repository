#pragma once


#include <Windows.h>


namespace Core
{
    class Mod
    {
    public:
        Mod(HMODULE module);
        virtual ~Mod();

        Mod(const Mod&) = delete;
        Mod(Mod&&) = delete;
        Mod& operator =(const Mod&) = delete;
        Mod& operator =(Mod&&) = delete;

    public:
        HMODULE GetModule() const;

        void StartLoading() const;
        void WaitTillLoaded(DWORD timeout = INFINITE) const;

    private:
        // To be defined in a derived class.
        virtual void Load() = 0;

    private:
        static DWORD CALLBACK LoadThreadProc(LPVOID lpParameter);

    private:
        HMODULE m_Module = nullptr;
        HANDLE m_LoadThread = nullptr;
    };
}