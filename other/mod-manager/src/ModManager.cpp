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

ImGuiManager& ModManager::GetImGuiManager()
{
    return m_ImGuiManager;
}

void ModManager::OnProcessAttach()
{
    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpParameter) -> DWORD
    {
        static_cast<ModManager*>(lpParameter)->Load();
        return 0;
    };
    m_LoadThread = CreateThread(nullptr, 0, loadThreadProc, this, 0, nullptr);
}

void ModManager::OnProcessDetach()
{
    WaitForSingleObject(m_LoadThread, 5000);

    m_ImGuiManager.Shutdown();
}

void ModManager::OnThreadAttach()
{
    HANDLE currentThread = GetCurrentThread();

    m_DetourHookManager.OnThreadAttach(currentThread);
}

void ModManager::OnThreadDetach()
{
    HANDLE currentThread = GetCurrentThread();

    m_DetourHookManager.OnThreadDetach(currentThread);
}

void ModManager::Load()
{
    // TODO: Wait till the game initializes globals.
    
    m_ImGuiManager.Initialize();
}