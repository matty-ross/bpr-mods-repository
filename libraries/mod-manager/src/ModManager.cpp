#include <cstring>
#include <exception>
#include <Windows.h>

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "core/Path.hpp"
#include "core/Logger.hpp"
#include "core/Patch.hpp"
#include "mod-manager/ModManager.hpp"
#include "mod-manager/HookManager.hpp"
#include "mod-manager/ImGuiManager.hpp"


ModManager ModManager::s_Instance;


ModManager::ModManager()
    :
    m_Logger(k_Name),
    m_ConfigDirectory("%LOCALAPPDATA%\\Criterion Games\\Burnout Paradise Remastered\\mods\\"),
    m_ModManagerConfigFile(m_ConfigDirectory, m_Logger),
    m_HookManager(m_Logger),
    m_ImGuiManager(m_ModManagerConfigFile.GetImGuiConfig(), m_ConfigDirectory, m_Logger)
{
}

ModManager& ModManager::Get()
{
    return s_Instance;
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

bool ModManager::CheckModVersion(const char* modVersion) const
{
    return strcmp(modVersion, k_Version) == 0;
}

void ModManager::OnProcessAttach()
{
    Core::Logger::Initialize();

    auto loadThreadProc = [](LPVOID) -> DWORD
    {
        s_Instance.Load();

        return 0;
    };
    m_LoadThreadHandle = CreateThread(nullptr, 0, loadThreadProc, nullptr, 0, nullptr);
}

void ModManager::OnProcessDetach()
{
    CloseHandle(m_LoadThreadHandle);
    Unload();
}

void ModManager::Load()
{
    try
    {
        if (!m_ConfigDirectory.Exists())
        {
            m_ConfigDirectory.CreateDirectoryTree();
            m_Logger.Info("Created config directory. path: '%s'", m_ConfigDirectory.GetPath());
        }

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

        m_ModManagerConfigFile.Load();
        m_HookManager.Load();
        m_ImGuiManager.Load();

        m_ImGuiManager.AddMenu([]()
        {
            s_Instance.RenderMenu();
        });
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
        m_ImGuiManager.Unload();
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void ModManager::RenderMenu()
{
    if (ImGui::Begin(k_Name, nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.0f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_Version);
        ImGui::Text("Author      %s", k_Author);
        ImGui::Text("Framerate   %.1f", io.Framerate);

        m_ImGuiManager.RenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
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
