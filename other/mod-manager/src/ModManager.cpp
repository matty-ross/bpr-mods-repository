#include "ModManager.h"


void ModManager::OnProcessAttach()
{
}

void ModManager::OnProcessDetach()
{
}

void ModManager::OnThreadAttach()
{
    HANDLE currentThread = GetCurrentThread();

    m_DetourHookManager.OnThreadAttached(currentThread);
}

void ModManager::OnThreadDetach()
{
    HANDLE currentThread = GetCurrentThread();

    m_DetourHookManager.OnThreadDetached(currentThread);
}