#include "ModManager.hpp"

#include "core/Pointer.hpp"


static constexpr char k_Name[] = "Mod Manager";


ModManager ModManager::s_Instance;


ModManager::ModManager()
    :
    m_Logger(k_Name),
    m_DetourPresent
    {
        .HookAddress    = Core::Pointer(0x00C89F90).GetAddress(),
        .DetourFunction = &ModManager::DetourPresent,
    },
    m_DetourWindowProc
    {
        .HookAddress    = Core::Pointer(0x008FB9E2).GetAddress(),
        .DetourFunction = &ModManager::DetourWindowProc,
    },
    m_DetourUpdateKeyboardState
    {
        .HookAddress    = Core::Pointer(0x0664BB29).GetAddress(),
        .DetourFunction = &ModManager::DetourUpdateKeyboardState,
    }
{
}

ModManager& ModManager::Get()
{
    return s_Instance;
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
    Unload();
    CloseHandle(m_LoadThread);
}

void ModManager::Load()
{
    try
    {
        m_Logger.Info("Loading...");

        // Wait to be at or past the Start Screen. 
        {
            m_Logger.Info("Waiting to be at or past the Start Screen...");
            
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
            
            m_Logger.Info("At or past the Start Screen.");
        }

        // Load ImGui manager.
        {
            m_Logger.Info("Loading ImGui manager...");
            
            m_ImGuiManager.Load();
            
            m_Logger.Info("Loaded ImGui manager.");
        }

        // Attach Present detour.
        {
            m_Logger.Info("Attaching Present detour...");
            
            m_DetourHookManager.AttachDetourHook(m_DetourPresent);
            
            m_Logger.Info("Attached Present detour.");
        }

        // Attach WindowProc detour.
        {
            m_Logger.Info("Attaching WindowProc detour...");
            
            m_DetourHookManager.AttachDetourHook(m_DetourWindowProc);
            
            m_Logger.Info("Attached WindowProc detour.");
        }

        // Attach UpdateKeyboardState detour.
        {
            m_Logger.Info("Attaching UpdateKeyboardState detour...");
            
            m_DetourHookManager.AttachDetourHook(m_DetourUpdateKeyboardState);
            
            m_Logger.Info("Attached UpdateKeyboardState detour.");
        }

        m_Logger.Info("Loaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(nullptr, e.what(), k_Name, MB_ICONERROR);
    }
}

void ModManager::Unload()
{
    try
    {
        m_Logger.Info("Unloading...");

        // Detach UpdateKeyboardState detour.
        {
            m_Logger.Info("Detaching UpdateKeyboardState detour...");
            
            m_DetourHookManager.DetachDetourHook(m_DetourUpdateKeyboardState);
            
            m_Logger.Info("Detached UpdateKeyboardState detour.");
        }

        // Detach WindowProc detour.
        {
            m_Logger.Info("Detaching WindowProc detour...");
            
            m_DetourHookManager.DetachDetourHook(m_DetourWindowProc);
            
            m_Logger.Info("Detached WindowProc detour.");
        }

        // Detach Present detour.
        {
            m_Logger.Info("Detaching Present detour...");
            
            m_DetourHookManager.DetachDetourHook(m_DetourPresent);
            
            m_Logger.Info("Detached Present detour.");
        }

        // Unload ImGui manager.
        {
            m_Logger.Info("Unloading ImGui manager...");
            
            m_ImGuiManager.Unload();
            
            m_Logger.Info("Unloaded ImGui manager.");
        }

        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(nullptr, e.what(), k_Name, MB_ICONERROR);
    }
}

__declspec(naked) void ModManager::DetourPresent()
{
    __asm
    {
        pushfd
        pushad

        mov ecx, dword ptr [s_Instance.m_ImGuiManager]
        call ImGuiManager::OnRenderFrame

        popad
        popfd
        
        jmp dword ptr [s_Instance.m_DetourPresent.HookAddress]
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
        mov ecx, dword ptr [s_Instance.m_ImGuiManager]
        call ImGuiManager::OnWindowMessage

        test al, al
        
        popad
        popfd
        
        jz _continue
        
        // Break from the switch statement.
        mov eax, 0x008FBC01
        jmp eax

    _continue:
        jmp dword ptr [s_Instance.m_DetourWindowProc.HookAddress]
    }
}

__declspec(naked) void ModManager::DetourUpdateKeyboardState()
{
    __asm
    {
        pushfd
        pushad

        mov ecx, dword ptr [s_Instance.m_ImGuiManager]
        call ImGuiManager::WantCaptureKeyboard

        test al, al
        jz _continue

        // Clear all keys.
        mov ecx, 0x100
        mov al, 0x0
        lea edi, [ebp - 0x100]
        rep stosb

    _continue:
        popad
        popfd
        
        jmp dword ptr [s_Instance.m_DetourUpdateKeyboardState.HookAddress]
    }
}
