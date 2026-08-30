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
    m_ConfigDirectoryPath(k_ConfigDirectoryPath),
    m_ModManagerConfigFile(m_ConfigDirectoryPath, m_Logger),
    m_HookManager(m_Logger),
    m_ImGuiManager(m_ModManagerConfigFile.GetImGuiConfig(), m_ConfigDirectoryPath, m_Logger)
{
}

ModManager& ModManager::Get()
{
    return s_Instance;
}

Core::Path ModManager::GetConfigDirectoryPath() const
{
    return m_ConfigDirectoryPath;
}

HookManager& ModManager::GetHookManager()
{
    return m_HookManager;
}

ImGuiManager& ModManager::GetImGuiManager()
{
    return m_ImGuiManager;
}

bool ModManager::CheckVersion(const char* version) const
{
    return strcmp(version, k_Version) == 0;
}

void ModManager::Load()
{
    try
    {
        Core::Logger::Initialize();

        if (!m_ConfigDirectoryPath.Exists())
        {
            m_ConfigDirectoryPath.CreateDirectoryTree();
            m_Logger.Info("Created config directory. path: '%s'", m_ConfigDirectoryPath.GetPath());
        }

        m_ModManagerConfigFile.Load();
        m_HookManager.Load();

        m_ImGuiManager.AddMenu([]() { s_Instance.RenderMenu(); });

        auto deferredLoadThreadProc = [](LPVOID) -> DWORD
        {
            s_Instance.DeferredLoad();

            return 0;
        };
        m_DeferredLoadThreadHandle = CreateThread(nullptr, 0, deferredLoadThreadProc, nullptr, 0, nullptr);
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

        CloseHandle(m_DeferredLoadThreadHandle);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void ModManager::DeferredLoad()
{
    try
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

        m_ImGuiManager.Load();
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

        if (ImGui::CollapsingHeader("Config"))
        {
            if (ImGui::Button("Save##mod-manager-config-file"))
            {
                m_ModManagerConfigFile.Save();
            }

            ImGui::SameLine();

            if (ImGui::Button("Load##mod-manager-config-file"))
            {
                m_ModManagerConfigFile.Load();
            }

            m_ImGuiManager.RenderMenu();
        }

        ImGui::PopItemWidth();
    }
    ImGui::End();
}
