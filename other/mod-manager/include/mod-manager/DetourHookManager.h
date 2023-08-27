#pragma once


#include <Windows.h>


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
    void BeginTransaction();
    void EndTransaction();
    bool IsTransactionInProgress() const;

private:
    CRITICAL_SECTION m_TransactionCriticalSection = {};
    bool m_TransactionInProgress = false;
};