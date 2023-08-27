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