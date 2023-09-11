#pragma once


#include <Windows.h>

#include "mod-manager/ModManagerApi.h"


struct DetourHook
{
    void* HookAddress;
    const void* DetourFunction;
    void* StubFunction;
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
    MOD_MANAGER_API void AttachDetourHook(DetourHook& detourHook, bool createStubFunction = true);
    MOD_MANAGER_API void DetachDetourHook(DetourHook& detourHook, bool destroyStubFunction = true);

    MOD_MANAGER_API void BeginTransaction();
    MOD_MANAGER_API void EndTransaction();
    MOD_MANAGER_API bool IsTransactionInProgress() const;

private:
    CRITICAL_SECTION m_TransactionCriticalSection = {};
    bool m_TransactionInProgress = false;
};