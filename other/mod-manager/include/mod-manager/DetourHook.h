#pragma once


#include "core/Pointer.h"

#include "mod-manager/ModManagerApi.h"


class MOD_MANAGER_API DetourHook
{
public:
    DetourHook(Core::Pointer hookAddress, const void* detourFunction);
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
    Core::Pointer m_Stub = nullptr;
        
    bool m_Attached = false;
};