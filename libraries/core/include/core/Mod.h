#pragma once


#include <Windows.h>


namespace Core
{
    class Mod
    {
    public:
        Mod(HMODULE module);
        virtual ~Mod() = default;

        Mod(const Mod&) = delete;
        Mod(Mod&&) = delete;
        Mod& operator =(const Mod&) = delete;
        Mod& operator =(Mod&&) = delete;

    public:
        HMODULE GetModule() const;

        virtual void OnProcessAttach();
        virtual void OnProcessDetach();
        virtual void OnThreadAttach();
        virtual void OnThreadDetach();

    private:
        virtual void Load() = 0;    // To be implemented in a derived class.
        virtual void Unload() = 0;  // To be implemented in a derived class.
    
    private:
        HMODULE m_Module = nullptr;

        HANDLE m_LoadThread = nullptr;
    };
}