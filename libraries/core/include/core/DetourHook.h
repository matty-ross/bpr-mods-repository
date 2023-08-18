#pragma once


#include "core/Pointer.h"


namespace Core
{
    class DetourHook
    {
    public:
        DetourHook(Pointer hookAddress, const void* detourFunction);
        ~DetourHook();

        DetourHook(const DetourHook&) = delete;
        DetourHook(DetourHook&&) = delete;
        DetourHook& operator =(const DetourHook&) = delete;
        DetourHook& operator =(DetourHook&&) = delete;

    public:
        void Attach();
        void Detach();
        bool IsAttached() const;

    private:
        void* m_HookAddress = nullptr;
        const void* m_DetourFunction = nullptr;
        Pointer m_Stub = nullptr;
        
        bool m_Attached = false;
    };
}