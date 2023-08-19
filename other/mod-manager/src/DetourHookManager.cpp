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
}

void DetourHookManager::EndTransaction()
{
    LeaveCriticalSection(&m_TransactionCriticalSection);
    m_TransactionInProgress = false;
}

bool DetourHookManager::IsTransactionInProgress() const
{
    return m_TransactionInProgress;
}

void DetourHookManager::OnThreadAttached(HANDLE thread)
{
    m_Threads.push_back(thread);
}