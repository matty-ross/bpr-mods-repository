#include "ModManager.h"


extern ModManager* g_ModManager;


ModManager::ModManager()
    :
    m_DetourPresent(0x00C89F90, &ModManager::DetourPresent),
    m_DetourWindowProc(0x008FB9E2, &ModManager::DetourWindowProc)
{
}

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
    CloseHandle(m_LoadThread);

    Unload();
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
    auto isAtOrPastStartScreenState = []() -> bool
    {
        Core::Pointer gameModule = 0x013FC8E0;
        
        return
            gameModule.as<void*>() != nullptr &&
            gameModule.deref().at(0xB6D4C8).as<int32_t>() >= 3
        ;
    };


    while (!isAtOrPastStartScreenState())
    {
        Sleep(1000);
    }
    
    m_ImGuiManager.Initialize();

    m_DetourPresent.Attach();
    m_DetourWindowProc.Attach();
}

void ModManager::Unload()
{
    m_DetourWindowProc.Detach();
    m_DetourPresent.Detach();
    
    m_ImGuiManager.Shutdown();
}

__declspec(naked) void ModManager::DetourPresent()
{
    // TODO

    __asm
    {
        ret
    }
}

__declspec(naked) void ModManager::DetourWindowProc()
{
    // TODO
    
    __asm
    {
        ret
    }
}