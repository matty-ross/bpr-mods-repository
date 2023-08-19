#pragma once


#include <set>

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
    void OnThreadDetached(HANDLE thread);

private:
    CRITICAL_SECTION m_TransactionCriticalSection = {};
    bool m_TransactionInProgress = false;

    std::set<HANDLE> m_Threads;
};