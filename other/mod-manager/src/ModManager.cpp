#include "ModManager.h"


extern ModManager* g_ModManager;


ModManager& ModManager::Get()
{
    return *g_ModManager;
}

DetourHookManager& ModManager::GetDetourHookManager()
{
    return m_DetourHookManager;
}

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