#include "DetourHookManager.hpp"

#include "vendor/detours.hpp"


// https://github.com/microsoft/Detours/wiki/Using-Detours


DetourHookManager::DetourHookManager()
{
    InitializeCriticalSection(&m_CriticalSection);
}

DetourHookManager::~DetourHookManager()
{
    DeleteCriticalSection(&m_CriticalSection);
}

CRITICAL_SECTION* DetourHookManager::GetCriticalSection()
{
    return &m_CriticalSection;
}

void DetourHookManager::Attach(DetourHook& detourHook)
{
    EnterCriticalSection(&m_CriticalSection);
    
    DetourTransactionBegin();
    DetourAttach(&detourHook.Target, detourHook.Detour);
    DetourTransactionCommit();
    
    LeaveCriticalSection(&m_CriticalSection);
}

void DetourHookManager::Detach(DetourHook& detourHook)
{
    EnterCriticalSection(&m_CriticalSection);
    
    DetourTransactionBegin();
    DetourDetach(&detourHook.Target, detourHook.Detour);
    DetourTransactionCommit();
    
    LeaveCriticalSection(&m_CriticalSection);
}
