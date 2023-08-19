#pragma once


#include <vector>

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

    void OnThreadAttached(HANDLE thread);

private:
    CRITICAL_SECTION m_TransactionCriticalSection = {};
    bool m_TransactionInProgress = false;

    std::vector<HANDLE> m_Threads;
};