#include "DetourHookManager.h"

#include "detours/detours.h"


DetourHookManager::DetourHookManager()
{
    InitializeCriticalSection(&m_TransactionCriticalSection);
}

DetourHookManager::~DetourHookManager()
{
    DeleteCriticalSection(&m_TransactionCriticalSection);
}

void DetourHookManager::BeginTransaction()
{
    EnterCriticalSection(&m_TransactionCriticalSection);
    m_TransactionInProgress = true;

    for (HANDLE thread : m_Threads)
    {
        DetourUpdateThread(thread);
    }

    DetourTransactionBegin();
}

void DetourHookManager::EndTransaction()
{
    DetourTransactionCommit();

    m_TransactionInProgress = false;
    LeaveCriticalSection(&m_TransactionCriticalSection);
}

bool DetourHookManager::IsTransactionInProgress() const
{
    return m_TransactionInProgress;
}

void DetourHookManager::OnThreadAttached(HANDLE thread)
{
    m_Threads.insert(thread);
}

void DetourHookManager::OnThreadDetached(HANDLE thread)
{
    m_Threads.erase(thread);
}