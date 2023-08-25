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

        virtual void OnProcessAttach() = 0;
        virtual void OnProcessDetach() = 0;
        virtual void OnThreadAttach(HANDLE thread) = 0;
        virtual void OnThreadDetach(HANDLE thread) = 0;
    
    private:
        HMODULE m_Module = nullptr;
    };
}