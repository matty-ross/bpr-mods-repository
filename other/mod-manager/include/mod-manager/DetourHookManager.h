#pragma once


#include <Windows.h>

#include "mod-manager/ModManagerApi.h"


class DetourHookManager
{
public:
    DetourHookManager();
    ~DetourHookManager();

    DetourHookManager(const DetourHookManager&) = delete;
    DetourHookManager(DetourHookManager&&) = delete;
    DetourHookManager& operator =(const DetourHookManager&) = delete;
    DetourHookManager& operator =(DetourHookManager&&) = delete;

public:
    MOD_MANAGER_API void BeginTransaction();
    MOD_MANAGER_API void EndTransaction();
    MOD_MANAGER_API bool IsTransactionInProgress() const;

private:
    CRITICAL_SECTION m_TransactionCriticalSection = {};
    bool m_TransactionInProgress = false;
};