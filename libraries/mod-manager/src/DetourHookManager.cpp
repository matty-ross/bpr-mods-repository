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

void DetourHookManager::BeginTransaction()
{
    EnterCriticalSection(&m_CriticalSection);
    DetourTransactionBegin();
}

void DetourHookManager::EndTransaction()
{
    DetourTransactionCommit();
    LeaveCriticalSection(&m_CriticalSection);
}

void DetourHookManager::AttachDetourHook(DetourHook& detourHook)
{
    BeginTransaction();
    DetourAttach(&detourHook.HookAddress, detourHook.DetourFunction);
    EndTransaction();
}

void DetourHookManager::DetachDetourHook(DetourHook& detourHook)
{
    BeginTransaction();
    DetourDetach(&detourHook.HookAddress, detourHook.DetourFunction);
    EndTransaction();
}
