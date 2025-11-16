#include "ModMenu.hpp"

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "mod-manager/ModManager.hpp"


static constexpr char k_ModName[]      = "Mod Menu";
static constexpr char k_ModVersion[]   = "1.3.0";
static constexpr char k_ModAuthor[]    = "PISros0724 (Matty)";
static constexpr char k_ModDirectory[] = "mod-menu\\";


ModMenu ModMenu::s_Instance;


ModMenu::ModMenu()
    :
    m_ConfigDirectory(ModManager::Get().GetConfigDirectory().Append(k_ModDirectory)),
    m_Logger(k_ModName),
    m_CustomColorsFile(m_ConfigDirectory, m_Logger),
    m_VehicleManager(m_CustomColorsFile),
    m_DetourDoUpdate
    {
        .Target = Core::Pointer(0x00A41B95).GetAddress(),
        .Detour = &ModMenu::DetourDoUpdate,
    },
    m_DetourPreWorldUpdate
    {
        .Target = Core::Pointer(0x00A2A512).GetAddress(),
        .Detour = &ModMenu::DetourPreWorldUpdate,
    },
    m_DetourUpdateHudFlow
    {
        .Target = Core::Pointer(0x066ECB48).GetAddress(),
        .Detour = &ModMenu::DetourUpdateHudFlow,
    },
    m_DetourUpdateActiveRaceVehicleColors
    {
        .Target = Core::Pointer(0x06A6DFFD).GetAddress(),
        .Detour = &ModMenu::DetourUpdateActiveRaceVehicleColors,
    },
    m_DetourCheckSwitchableBoost
    {
        .Target = Core::Pointer(0x0099CF04).GetAddress(),
        .Detour = &ModMenu::DetourCheckSwitchableBoost,
    },
    m_Menu
    {
        .OnRenderFunction = [&]() { OnRenderMenu(); },
    }
{
}

ModMenu& ModMenu::Get()
{
    return s_Instance;
}

void ModMenu::OnProcessAttach()
{
    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpThreadParameter) -> DWORD
    {
        s_Instance.Load();
        return 0;
    };
    m_LoadThreadHandle = CreateThread(nullptr, 0, loadThreadProc, nullptr, 0, nullptr);
}

void ModMenu::OnProcessDetach()
{
    Unload();
    CloseHandle(m_LoadThreadHandle);
}

void ModMenu::Load()
{
    try
    {
        m_Logger.Info("Loading...");

        // Check mod version.
        {
            m_Logger.Info("Checking mod version...");

            if (!ModManager::Get().CheckModVersion(k_ModVersion))
            {
                throw std::exception("Mod Manager and Mod versions mismatch.");
            }

            m_Logger.Info("Checked mod version.");
        }

        // Create mod config directory.
        {
            m_Logger.Info("Creating mod config directory '%s' ...", m_ConfigDirectory.GetPath());

            m_ConfigDirectory.Create();

            m_Logger.Info("Created mod config directory.");
        }

        // Load custom colors.
        {
            m_CustomColorsFile.Load();
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

        // Load vehicles.
        {
            m_Logger.Info("Loading vehicles...");

            m_VehicleManager.LoadVehicles();

            m_Logger.Info("Vehicles loaded.");
        }
        
        // Load wheels.
        {
            m_Logger.Info("Loading wheels...");

            m_VehicleManager.LoadWheels();

            m_Logger.Info("Wheels loaded.");
        }

        // Attach DoUpdate detour.
        {
            m_Logger.Info("Attaching DoUpdate detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourDoUpdate);

            m_Logger.Info("Attached DoUpdate detour.");
        }

        // Attach PreWorldUpdate detour.
        {
            m_Logger.Info("Attaching PreWorldUpdate detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourPreWorldUpdate);

            m_Logger.Info("Attached PreWorldUpdate detour.");
        }

        // Attach UpdateHudFlow detour.
        {
            m_Logger.Info("Attaching UpdateHudFlow detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourUpdateHudFlow);

            m_Logger.Info("Attached UpdateHudFlow detour.");
        }

        // Attach UpdateActiveRaceVehicleColors detour.
        {
            m_Logger.Info("Attaching UpdateActiveRaceVehicleColors detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourUpdateActiveRaceVehicleColors);

            m_Logger.Info("Attached UpdateActiveRaceVehicleColors detour.");
        }

        // Attach CheckSwitchableBoost detour.
        {
            m_Logger.Info("Attaching CheckSwitchableBoost detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourCheckSwitchableBoost);

            m_Logger.Info("Attached CheckSwitchableBoost detour.");
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

void ModMenu::Unload()
{
    try
    {
        m_Logger.Info("Unloading...");

        // Save custom colors.
        {
            m_CustomColorsFile.Save();
        }

        // Remove menu.
        {
            m_Logger.Info("Removing menu...");

            ModManager::Get().GetImGuiManager().RemoveMenu(&m_Menu);

            m_Logger.Info("Removed menu.");
        }

        // Detach CheckSwitchableBoost detour.
        {
            m_Logger.Info("Detaching CheckSwitchableBoost detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourCheckSwitchableBoost);

            m_Logger.Info("Detached CheckSwitchableBoost detour.");
        }

        // Detach UpdateActiveRaceVehicleColors detour.
        {
            m_Logger.Info("Detaching UpdateActiveRaceVehicleColors detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourUpdateActiveRaceVehicleColors);

            m_Logger.Info("Detached UpdateActiveRaceVehicleColors detour.");
        }

        // Detach UpdateHudFlow detour.
        {
            m_Logger.Info("Detaching UpdateHudFlow detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourUpdateHudFlow);

            m_Logger.Info("Detached UpdateHudFlow detour.");
        }

        // Detach PreWorldUpdate detour.
        {
            m_Logger.Info("Detaching PreWorldUpdate detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourPreWorldUpdate);

            m_Logger.Info("Detached PreWorldUpdate detour.");
        }

        // Detach DoUpdate detour.
        {
            m_Logger.Info("Detaching DoUpdate detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourDoUpdate);

            m_Logger.Info("Detached DoUpdate detour.");
        }

        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
    }
}

void ModMenu::OnDoUpdate()
{
    m_VehicleManager.OnDoUpdate();
}

void ModMenu::OnPreWorldUpdate(void* gameEventQueue, void* gameActionQueue)
{
    m_Misc.OnPreWorldUpdate(gameEventQueue, gameActionQueue);
    m_VehicleManager.OnPreWorldUpdate(gameEventQueue, gameActionQueue);
}

void ModMenu::OnUpdateHudFlow()
{
    m_Misc.OnUpdateHudFlow();
}

void ModMenu::OnUpdateActiveRaceVehicleColors()
{
    m_VehicleManager.OnUpdateActiveRaceVehicleColors();
}

void ModMenu::OnRenderMenu()
{
    if (ImGui::Begin(k_ModName, nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.0f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_ModVersion);
        ImGui::Text("Author      %s", k_ModAuthor);
        ImGui::Text("Framerate   %.1f", io.Framerate);

        m_Misc.OnRenderMenu();
        m_Environment.OnRenderMenu();
        m_VehicleManager.OnRenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}

__declspec(naked) void ModMenu::DetourDoUpdate()
{
    __asm
    {
        pushfd
        pushad

        mov ecx, offset s_Instance
        call ModMenu::OnDoUpdate

        popad
        popfd

        jmp dword ptr [s_Instance.m_DetourDoUpdate.Target]
    }
}

__declspec(naked) void ModMenu::DetourPreWorldUpdate()
{
    __asm
    {
        pushfd
        pushad

        push esi // BrnGameState::GameStateModuleIO::BaseGameActionQueue<13312>*
        push eax // BrnGameState::GameStateModuleIO::GameEventQueue*
        mov ecx, offset s_Instance
        call ModMenu::OnPreWorldUpdate

        popad
        popfd

        jmp dword ptr [s_Instance.m_DetourPreWorldUpdate.Target]
    }
}

__declspec(naked) void ModMenu::DetourUpdateHudFlow()
{
    __asm
    {
        pushfd
        pushad

        mov ecx, offset s_Instance
        call ModMenu::OnUpdateHudFlow

        popad
        popfd

        jmp dword ptr [s_Instance.m_DetourUpdateHudFlow.Target]
    }
}

__declspec(naked) void ModMenu::DetourUpdateActiveRaceVehicleColors()
{
    __asm
    {
        pushfd
        pushad

        mov ecx, offset s_Instance
        call ModMenu::OnUpdateActiveRaceVehicleColors

        popad
        popfd

        jmp dword ptr [s_Instance.m_DetourUpdateActiveRaceVehicleColors.Target]
    }
}

__declspec(naked) void ModMenu::DetourCheckSwitchableBoost()
{
    __asm
    {
        cmp byte ptr [s_Instance.m_VehicleManager.m_OverrideSwitchableBoost], 0
        je _continue

        // Set switchable boost flag.
        or ecx, 0x100

    _continue:
        jmp dword ptr [s_Instance.m_DetourCheckSwitchableBoost.Target]
    }
}
