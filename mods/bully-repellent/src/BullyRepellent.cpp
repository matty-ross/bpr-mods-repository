#include "BullyRepellent.hpp"

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "mod-manager/ModManager.hpp"


static constexpr char k_ModName[]      = "Bully Repellent";
static constexpr char k_ModVersion[]   = "1.3.0";
static constexpr char k_ModAuthor[]    = "PISros0724 (Matty)";
static constexpr char k_ModDirectory[] = "bully-repellent\\";


BullyRepellent BullyRepellent::s_Instance;


BullyRepellent::BullyRepellent()
    :
    m_ConfigDirectory(ModManager::Get().GetConfigDirectory().Append(k_ModDirectory)),
    m_Logger(k_ModName),
    m_BlacklistedPlayersFile(m_ConfigDirectory, m_Logger),
    m_OnlinePlayers(m_BlacklistedPlayersFile),
    m_DetourOnGuiEventNetworkPlayerStatus
    {
        .Target = Core::Pointer(0x0092BECB).GetAddress(),
        .Detour = &BullyRepellent::DetourOnGuiEventNetworkPlayerStatus,
    },
    m_Menu
    {
        .OnRenderFunction = [&]() { OnRenderMenu(); },
    }
{
}

BullyRepellent& BullyRepellent::Get()
{
    return s_Instance;
}

void BullyRepellent::OnProcessAttach()
{
    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpThreadParameter) -> DWORD
    {
        s_Instance.Load();
        return 0;
    };
    m_LoadThreadHandle = CreateThread(nullptr, 0, loadThreadProc, nullptr, 0, nullptr);
}

void BullyRepellent::OnProcessDetach()
{
    Unload();
    CloseHandle(m_LoadThreadHandle);
}

void BullyRepellent::Load()
{
    try
    {
        m_Logger.Info("Loading...");

        // Check versions.
        {
            m_Logger.Info("Checking versions...");

            if (strcmp(ModManager::GetVersion(), k_ModVersion) != 0)
            {
                throw std::exception("Mod Manager and Mod versions mismatch.");
            }

            m_Logger.Info("Checked versions.");
        }

        // Create mod config directory.
        {
            m_Logger.Info("Creating mod config directory '%s' ...", m_ConfigDirectory.GetPath());

            m_ConfigDirectory.Create();

            m_Logger.Info("Created mod config directory.");
        }

        // Load blacklisted players.
        {
            m_BlacklistedPlayersFile.Load();
        }

        // Wait to be in game.
        {
            m_Logger.Info("Waiting to be in game...");
            
            while (true)
            {
                // BrnGameMainFlowController::GameMainFlowController::meCurrentState
                Core::Pointer gameModule = 0x013FC8E0;
                if (
                    gameModule.as<void*>() != nullptr &&
                    gameModule.deref().at(0xB6D4C8).as<int32_t>() == 6 // BrnGameMainFlowController::EMainGameFlowState::E_MGS_IN_GAME
                )
                {
                    break;
                }

                Sleep(1000);
            }
 
            m_Logger.Info("In game.");
        }

        // Attach OnGuiEventNetworkPlayerStatus detour.
        {
            m_Logger.Info("Attaching OnGuiEventNetworkPlayerStatus detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourOnGuiEventNetworkPlayerStatus);

            m_Logger.Info("Attached OnGuiEventNetworkPlayerStatus detour.");
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
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
    }
}

void BullyRepellent::Unload()
{
    try
    {
        m_Logger.Info("Unloading...");

        // Save blacklisted players.
        {
            m_BlacklistedPlayersFile.Save();
        }

        // Remove menu.
        {
            m_Logger.Info("Removing menu...");

            ModManager::Get().GetImGuiManager().RemoveMenu(&m_Menu);

            m_Logger.Info("Removed menu.");
        }

        // Detach OnGuiEventNetworkPlayerStatus detour.
        {
            m_Logger.Info("Detaching OnGuiEventNetworkPlayerStatus detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourOnGuiEventNetworkPlayerStatus);

            m_Logger.Info("Detached OnGuiEventNetworkPlayerStatus detour.");
        }

        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
    }
}

void BullyRepellent::OnGuiEventNetworkPlayerStatus(void* guiEventNetworkPlayerStatus, void* guiCache)
{
    m_OnlinePlayers.OnGuiEventNetworkPlayerStatus(guiEventNetworkPlayerStatus, guiCache);
}

void BullyRepellent::OnRenderMenu()
{
    if (ImGui::Begin(k_ModName, nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.0f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_ModVersion);
        ImGui::Text("Author      %s", k_ModAuthor);
        ImGui::Text("Framerate   %.1f", io.Framerate);

        m_OnlinePlayers.OnRenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}

__declspec(naked) void BullyRepellent::DetourOnGuiEventNetworkPlayerStatus()
{
    __asm
    {
        pushfd
        pushad

        push ebx // BrnGui::GuiCache*
        push ecx // BrnGui::GuiEventNetworkPlayerStatus*
        mov ecx, offset s_Instance
        call BullyRepellent::OnGuiEventNetworkPlayerStatus
        
        popad
        popfd
        
        jmp dword ptr [s_Instance.m_DetourOnGuiEventNetworkPlayerStatus.Target]
    }
}
