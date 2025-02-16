#pragma once


#include <Windows.h>

#include "mod-manager/ModManagerApi.hpp"


struct DetourHook
{
    void* Target;
    void* Detour;
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
    MOD_MANAGER_API CRITICAL_SECTION* GetCriticalSection();
    
    MOD_MANAGER_API void Attach(DetourHook& detourHook);
    MOD_MANAGER_API void Detach(DetourHook& detourHook);

private:
    CRITICAL_SECTION m_CriticalSection = {};
};
