#include <exception>
#include <Windows.h>

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "mod-manager/ModManager.hpp"

#include "BullyRepellent.hpp"


BullyRepellent BullyRepellent::s_Instance;


BullyRepellent::BullyRepellent()
    :
    m_Logger(k_Name),
    m_ConfigDirectoryPath(ModManager::Get().GetConfigDirectoryPath().Append(k_ConfigDirectoryPath)),
    m_BlacklistedPlayersFile(m_ConfigDirectoryPath, m_Logger),
    m_OnlinePlayers(m_BlacklistedPlayersFile)
{
}

BullyRepellent& BullyRepellent::Get()
{
    return s_Instance;
}

void BullyRepellent::Load()
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

        m_BlacklistedPlayersFile.Load();

        ModManager::Get().GetHookManager().AddGuiEventInterpreterUpdateHook(
            [](Core::Pointer guiOutEventQueue)
            {
                s_Instance.m_OnlinePlayers.CheckPlayers(guiOutEventQueue);
            }
        );

        ModManager::Get().GetImGuiManager().AddMenu([]() { s_Instance.RenderMenu(); });
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void BullyRepellent::Unload()
{
    try
    {
        m_BlacklistedPlayersFile.Save();
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void BullyRepellent::RenderMenu()
{
    if (ImGui::Begin(k_Name, nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.0f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_Version);
        ImGui::Text("Author      %s", k_Author);
        ImGui::Text("Framerate   %.1f", io.Framerate);

        m_OnlinePlayers.RenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}
