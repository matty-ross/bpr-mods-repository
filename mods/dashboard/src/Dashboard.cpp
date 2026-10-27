#include <cstdint>
#include <exception>
#include <Windows.h>

#include <vendor/imgui.hpp>

#include <core/Pointer.hpp>
#include <mod-manager/ModManager.hpp>

#include "Dashboard.hpp"


Dashboard Dashboard::s_Instance;


Dashboard::Dashboard()
    :
    m_Logger(k_Name),
    m_ConfigDirectoryPath(ModManager::Get().GetConfigDirectoryPath().Append(k_ConfigDirectoryPath)),
    m_AssetsDirectoryPath(".\\mods\\dashboard\\"),
    m_DashboardConfigFile(m_ConfigDirectoryPath, m_Logger),
    m_DashboardHud(m_DashboardConfigFile, m_AssetsDirectoryPath, m_Logger)
{
}

Dashboard& Dashboard::Get()
{
    return s_Instance;
}

void Dashboard::Load()
{
    try
    {
        if (!ModManager::Get().CheckVersion(k_Version))
        {
            throw std::exception("Mod Manager and Mod versions mismatch.");
        }

        if (!m_ConfigDirectoryPath.Exists())
        {
            m_ConfigDirectoryPath.CreateDirectoryTree();
            m_Logger.Info("Created config directory. path: '%s'", m_ConfigDirectoryPath.GetPath());
        }
        
        m_DashboardConfigFile.Load();
        
        m_DashboardHud.Load();

        auto deferredLoadThreadProc = [](LPVOID) -> DWORD
        {
            s_Instance.DeferredLoad();
            return 0;
        };
        HANDLE deferredLoadThreadHandle = CreateThread(nullptr, 0, deferredLoadThreadProc, nullptr, 0, nullptr);
        if (deferredLoadThreadHandle != NULL)
        {
            CloseHandle(deferredLoadThreadHandle);
        }
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void Dashboard::Unload()
{
    try
    {
        m_DashboardConfigFile.Save();
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void Dashboard::DeferredLoad()
{
    try
    {
        while (true)
        {
            Core::Pointer gameModule = 0x013FC8E0;
            if (gameModule.as<void*>() != nullptr)
            {
                int32_t mainGameFlowState = gameModule.deref().at(0xB6D4C8).as<int32_t>();
                if (mainGameFlowState == 6) // BrnGameMainFlowController::EMainGameFlowState::E_MGS_IN_GAME
                {
                    break;
                }
            }

            Sleep(1000);
        }

        EnterCriticalSection(ModManager::Get().GetImGuiManager().GetCriticalSection());
        m_DashboardHud.LoadTexture();
        m_DashboardHud.LoadFont();
        LeaveCriticalSection(ModManager::Get().GetImGuiManager().GetCriticalSection());

        ModManager::Get().GetImGuiManager().AddMenu([]() { s_Instance.RenderMenu(); });
        ModManager::Get().GetImGuiManager().AddOverlay([]() { s_Instance.m_DashboardHud.RenderOverlay(); });
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void Dashboard::RenderMenu()
{
    if (ImGui::Begin(k_Name, nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.0f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_Version);
        ImGui::Text("Author      %s", k_Author);
        ImGui::Text("Framerate   %.1f", io.Framerate);

        m_DashboardHud.RenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}
