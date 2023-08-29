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
    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpThreadParameter) -> DWORD
    {
        static_cast<ModManager*>(lpThreadParameter)->Load();
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
}

void ModManager::OnThreadDetach()
{
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
    __asm
    {
        pushfd
        pushad

        call ModManager::Get
        mov ecx, eax
        call ModManager::GetImGuiManager
        mov ecx, eax
        call ImGuiManager::OnRenderFrame

        popad
        popfd
        ret
    }
}

__declspec(naked) void ModManager::DetourWindowProc()
{
    __asm
    {
        pushfd
        pushad

        push dword ptr [ebp + 0x14]
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xC]
        push dword ptr [ebp + 0x8]

        call ModManager::Get
        mov ecx, eax
        call ModManager::GetImGuiManager
        mov ecx, eax
        call ImGuiManager::OnWindowMessage

        test al, al
        jz _continue

        // Break from the switch statement.
        popad
        popfd
        add esp, 0x4
        mov eax, 0x008FBC01
        jmp eax

    _continue:
        popad
        popfd
        ret
    }
}