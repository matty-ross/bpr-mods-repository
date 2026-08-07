#include <exception>
#include <Windows.h>

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "core/Logger.hpp"
#include "core/Patch.hpp"
#include "mod-manager/ModManager.hpp"
#include "mod-manager/HookManager.hpp"


ModManager ModManager::s_Instance;


ModManager::ModManager()
    :
    m_Logger(k_Name),
    m_ConfigDirectory("%LOCALAPPDATA%\\Criterion Games\\Burnout Paradise Remastered\\mods\\"),
    m_ModManagerConfigFile(m_ConfigDirectory, m_Logger),
    m_ImGuiManager(m_ConfigDirectory, m_ModManagerConfigFile.GetImGuiConfig())
{
}

ModManager& ModManager::Get()
{
    return s_Instance;
}

bool ModManager::CheckModVersion(const char* modVersion) const
{
    return strcmp(modVersion, k_Version) == 0;
}

Core::Path ModManager::GetConfigDirectory() const
{
    return m_ConfigDirectory;
}

HookManager& ModManager::GetHookManager()
{
    return m_HookManager;
}

ImGuiManager& ModManager::GetImGuiManager()
{
    return m_ImGuiManager;
}

void ModManager::Load()
{
    try
    {
        Core::Logger::Initialize();

        //m_ConfigDirectory.CreateDirectoryTree();
        //m_Logger.Info("Created config directory.");

        m_ModManagerConfigFile.Load();

        Core::Patch(0x0817E440, 6, m_Logger).WriteJMP(Hook_ImGuiRender);
        Core::Patch(0x008FB9D9, 5, m_Logger).WriteJMP(Hook_ImGuiWindowProc);
        Core::Patch(0x00A49235, 5, m_Logger).WriteJMP(Hook_ExecuteUpdateHooks);

        m_ImGuiManager.AddMenu([]()
        {
            ImGui::ShowDemoWindow();
        });

        auto loadThreadProc = [](LPVOID) -> DWORD
        {
            while (true)
            {
                Core::Pointer gameModule = 0x013FC8E0;
                if (gameModule.as<void*>() != nullptr)
                {
                    int32_t gameUpdateStage = gameModule.deref().at(0xB6D464).as<int32_t>();
                    if (gameUpdateStage == 1) // BrnGame::BrnGameModule::E_GAMEUPDATESTAGE_MAIN
                    {
                        break;
                    }
                }

                Sleep(1000);
            }

            s_Instance.m_ImGuiManager.Load();
            s_Instance.m_Logger.Info("Loaded ImGui.");

            return 0;
        };
        m_LoadThreadHandle = CreateThread(nullptr, 0, loadThreadProc, nullptr, 0, nullptr);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void ModManager::Unload()
{
    try
    {
        m_ModManagerConfigFile.Save();
        
        CloseHandle(m_LoadThreadHandle);
        m_ImGuiManager.Unload();
        s_Instance.m_Logger.Info("Unloaded ImGui.");
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

__declspec(naked) void ModManager::Hook_ImGuiRender()
{
    __asm
    {
        pushfd
        pushad
    
        mov ecx, offset ModManager::s_Instance.m_ImGuiManager
        call ImGuiManager::Render
    
        popad
        popfd
            
        // Original code.
        mov edx, dword ptr [ecx]
        push esi
        mov eax, dword ptr [edx + 0x8]

        // Jump back.
        push 0x0817E446
        ret
    }
}

__declspec(naked) void ModManager::Hook_ImGuiWindowProc()
{
    __asm
    {
        pushfd
        pushad

        push dword ptr [ebp + 0x14]
        push dword ptr [ebp + 0x10]
        push dword ptr [ebp + 0xC]
        push dword ptr [ebp + 0x8]
        mov ecx, offset ModManager::s_Instance.m_ImGuiManager
        call ImGuiManager::WindowProc
        
        popad
        popfd
        
        // Original code.
        push ebx
        mov ebx, dword ptr [ebp + 0xC]
        push esi

        // Jump back.
        push 0x008FB9DE
        ret
    }
}

__declspec(naked) void ModManager::Hook_ExecuteUpdateHooks()
{
    __asm
    {
        pushfd
        pushad

        mov ecx, offset ModManager::s_Instance.m_HookManager
        call HookManager::ExecuteUpdateHooks

        popad
        popfd

        // Original code.
        mov eax, dword ptr ds:[0x013FC8E0]

        // Jump back.
        push 0x00A4923A
        ret
    }
}



////0x0664BB29
//__declspec(naked) void ModManager::DetourUpdateKeyboardState()
//{
//    __asm
//    {
//        pushfd
//        pushad
//
//        call ImGui::GetIO
//        
//        cmp byte ptr [eax]ImGuiIO.WantCaptureKeyboard, 0
//        je _continue
//
//        // Clear all keys.
//        mov ecx, 0x100
//        mov al, 0
//        lea edi, [ebp - 0x100]
//        rep stosb
//
//    _continue:
//        popad
//        popfd
//        
//        jmp dword ptr [s_Instance.m_DetourUpdateKeyboardState.Target]
//    }
//}
