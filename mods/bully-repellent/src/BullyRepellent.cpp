#include "BullyRepellent.h"

#include "imgui/imgui.h"

#include "core/Pointer.h"

#include "mod-manager/ModManager.h"


static constexpr char k_ModName[]      = "Bully Repellent";
static constexpr char k_ModVersion[]   = "1.0.0";
static constexpr char k_ModAuthor[]    = "PISros0724 (Matty)";
static constexpr char k_ModDirectory[] = ".\\mods\\bully-repellent\\";


BullyRepellent::BullyRepellent(HMODULE module)
    :
    Mod(module),
    m_Logger(k_ModName),
    m_Menu
    {
        .OnRenderMenuFunction = [this]() { OnRenderMenu(); },
        .ToggleVisibilityHotkey = VK_F7,
        .Visible = true,
    }
{
}

void BullyRepellent::Load()
{
    try
    {
        m_Logger.Info("Loading...");

        // Wait to be in game.
        {
            m_Logger.Info("Waiting to be in game...");
            
            auto isInGameState = []() -> bool
            {
                Core::Pointer gameModule = 0x013FC8E0;

                return
                    gameModule.as<void*>() != nullptr &&
                    gameModule.deref().at(0xB6D4C8).as<int32_t>() == 6
                ;
            };
            
            while (!isInGameState())
            {
                Sleep(1000);
            }
 
            m_Logger.Info("In game.");
        }

        // Add menu.
        {
            m_Logger.Info("Adding menu...");

            ModManager::Get().GetImGuiManager().AddMenu(&m_Menu);

            m_Logger.Info("Added menu.");
        }

        m_Logger.Info("Loaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(nullptr, e.what(), k_ModName, MB_ICONERROR);
    }
}

void BullyRepellent::Unload()
{
    try
    {
        m_Logger.Info("Unloading...");



        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(nullptr, e.what(), k_ModName, MB_ICONERROR);
    }
}

void BullyRepellent::OnRenderMenu()
{
    if (ImGui::Begin(k_ModName))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_ModVersion);
        ImGui::Text("Author      %s", k_ModAuthor);
        ImGui::Text("Framerate   %.3f", io.Framerate);

        m_CurrentLobby.OnRenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}