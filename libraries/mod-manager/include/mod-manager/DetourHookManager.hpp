#pragma once


#include <Windows.h>

#include "mod-manager/ModManagerApi.hpp"


struct DetourHook
{
    void* HookAddress;
    void* DetourFunction;
};


class DetourHookManager
{
    friend class ModManager;

private:
    DetourHookManager();
    ~DetourHookManager();

public:
    DetourHookManager(const DetourHookManager&) = delete;
    DetourHookManager(DetourHookManager&&) = delete;
    DetourHookManager& operator =(const DetourHookManager&) = delete;
    DetourHookManager& operator =(DetourHookManager&&) = delete;

public:
    MOD_MANAGER_API void BeginTransaction();
    MOD_MANAGER_API void EndTransaction();
    
    MOD_MANAGER_API void AttachDetourHook(DetourHook& detourHook);
    MOD_MANAGER_API void DetachDetourHook(DetourHook& detourHook);

private:
    CRITICAL_SECTION m_CriticalSection = {};
};
