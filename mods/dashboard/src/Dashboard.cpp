#include "Dashboard.hpp"

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "mod-manager/ModManager.hpp"


using namespace std::string_literals;


static constexpr char k_ModName[]      = "Dashboard";
static constexpr char k_ModVersion[]   = "1.1.0";
static constexpr char k_ModAuthor[]    = "PISros0724 (Matty)";
static constexpr char k_ModDirectory[] = ".\\mods\\dashboard\\";


Dashboard Dashboard::s_Instance;


Dashboard::Dashboard()
    :
    m_Logger(k_ModName),
    m_DashboardConfigFile(k_ModDirectory + "dashboard-config.yaml"s, m_Logger),
    m_DashboardHud(m_DashboardConfigFile, m_Logger),
    m_DetourProgressionAddDistanceDriven
    {
        .Target = Core::Pointer(0x06E6B397).GetAddress(),
        .Detour = &Dashboard::DetourProgressionAddDistanceDriven,
    },
    m_Menu
    {
        .OnRenderFunction = [&]() { OnRenderMenu(); },
    },
    m_Overlay
    {
        .OnRenderFunction = [&]() { OnRenderOverlay(); },
    }
{
}

Dashboard& Dashboard::Get()
{
    return s_Instance;
}

void Dashboard::OnProcessAttach()
{
    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpThreadParameter) -> DWORD
    {
        static_cast<Dashboard*>(lpThreadParameter)->Load();
        return 0;
    };
    m_LoadThreadHandle = CreateThread(nullptr, 0, loadThreadProc, this, 0, nullptr);
}

void Dashboard::OnProcessDetach()
{
    Unload();
    CloseHandle(m_LoadThreadHandle);
}

void Dashboard::Load()
{
    try
    {
        m_Logger.Info("Loading...");

        // Check versions
        {
            m_Logger.Info("Checking versions...");

            if (strcmp(ModManager::GetVersion(), k_ModVersion) != 0)
            {
                throw std::exception("Mod Manager and Mod versions mismatch.");
            }

            m_Logger.Info("Checked versions.");
        }

        // Load dashboard config.
        {
            m_DashboardConfigFile.Load();
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

        // Load dashboard assets.
        {
            EnterCriticalSection(&ModManager::Get().GetImGuiManager().GetCriticalSection());
            
            m_DashboardHud.LoadTexture(k_ModDirectory + "texture.dds"s);
            m_DashboardHud.LoadFonts(k_ModDirectory + "font.ttf"s);
            
            ImGui_ImplDX11_InvalidateDeviceObjects();
            
            LeaveCriticalSection(&ModManager::Get().GetImGuiManager().GetCriticalSection());
        }

        // Attach ProgressionAddDistanceDriven detour.
        {
            m_Logger.Info("Attaching ProgressionAddDistanceDriven detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourProgressionAddDistanceDriven);

            m_Logger.Info("Attached ProgressionAddDistanceDriven detour.");
        }

        // Add menu.
        {
            m_Logger.Info("Adding menu...");

            ModManager::Get().GetImGuiManager().AddMenu(&m_Menu);

            m_Logger.Info("Added menu.");
        }

        // Add overlay.
        {
            m_Logger.Info("Adding overlay...");

            ModManager::Get().GetImGuiManager().AddOverlay(&m_Overlay);

            m_Logger.Info("Added overlay.");
        }

        m_Logger.Info("Loaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
    }
}

void Dashboard::Unload()
{
    try
    {
        m_Logger.Info("Unloading...");

        // Save dashboard config.
        {
            m_DashboardConfigFile.Save();
        }

        // Remove overlay.
        {
            m_Logger.Info("Removing overlay...");

            ModManager::Get().GetImGuiManager().RemoveOverlay(&m_Overlay);

            m_Logger.Info("Removed overlay.");
        }

        // Remove menu.
        {
            m_Logger.Info("Removing menu...");

            ModManager::Get().GetImGuiManager().RemoveMenu(&m_Menu);

            m_Logger.Info("Removed menu.");
        }

        // Detach ProgressionAddDistanceDriven detour.
        {
            m_Logger.Info("Detaching ProgressionAddDistanceDriven detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourProgressionAddDistanceDriven);

            m_Logger.Info("Detached ProgressionAddDistanceDriven detour.");
        }

        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
    }
}

void Dashboard::OnProgressionAddDistanceDriven(float distance, int32_t vehicleType)
{
    m_DashboardHud.OnProgressionAddDistanceDriven(distance, vehicleType);
}

void Dashboard::OnRenderMenu()
{
    if (ImGui::Begin(k_ModName, nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.0f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_ModVersion);
        ImGui::Text("Author      %s", k_ModAuthor);
        ImGui::Text("Framerate   %.3f", io.Framerate);

        m_DashboardHud.OnRenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}

void Dashboard::OnRenderOverlay()
{
    m_DashboardHud.OnRenderOverlay();
}

__declspec(naked) void Dashboard::DetourProgressionAddDistanceDriven()
{
    __asm
    {
        pushfd
        pushad

        sub esp, 4
        movss [esp], xmm1

        push esi // vehicle type
        push dword ptr [esp + 0x4] // distance
        mov ecx, offset s_Instance
        call Dashboard::OnProgressionAddDistanceDriven

        movss xmm1, [esp]
        add esp, 4

        popad
        popfd
        
        jmp dword ptr [s_Instance.m_DetourProgressionAddDistanceDriven.Target]
    }
}
