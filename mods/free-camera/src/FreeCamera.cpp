#include "FreeCamera.hpp"

#include <hidusage.h>

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "mod-manager/ModManager.hpp"


using namespace std::string_literals;


static constexpr char k_ModName[]      = "Free Camera";
static constexpr char k_ModVersion[]   = "1.0.0";
static constexpr char k_ModAuthor[]    = "PISros0724 (Matty)";
static constexpr char k_ModDirectory[] = ".\\mods\\free-camera\\";


FreeCamera FreeCamera::s_Instance;


FreeCamera::FreeCamera()
    :
    m_Logger(k_ModName),
    m_CustomParamtersFile(k_ModDirectory + "custom-parameters.yaml"s, m_Logger),
    m_GameplayExternalCamera(m_CustomParamtersFile),
    m_DetourArbitratorUpdate
    {
        .Target = Core::Pointer(0x009645E0).GetAddress(),
        .Detour = &FreeCamera::DetourArbitratorUpdate,
    },
    m_DetourSetPlayerVehicleIndex
    {
        .Target = Core::Pointer(0x00A3393B).GetAddress(),
        .Detour = &FreeCamera::DetourSetPlayerVehicleIndex,
    },
    m_Menu
    {
        .OnRenderFunction = [&]() { OnRenderMenu(); },
    }
{
}

FreeCamera& FreeCamera::Get()
{
    return s_Instance;
}

void FreeCamera::OnProcessAttach()
{
    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpThreadParameter) -> DWORD
    {
        static_cast<FreeCamera*>(lpThreadParameter)->Load();
        return 0;
    };
    m_LoadThreadHandle = CreateThread(nullptr, 0, loadThreadProc, this, 0, nullptr);
}

void FreeCamera::OnProcessDetach()
{
    Unload();
    CloseHandle(m_LoadThreadHandle);
}

void FreeCamera::Load()
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

        // Create mod directory.
        {
            m_Logger.Info("Creating mod directory '%s' ...", k_ModDirectory);
            
            CreateDirectoryA(k_ModDirectory, nullptr);

            m_Logger.Info("Created mod directory.");
        }

        // Load custom parameters.
        {
            m_CustomParamtersFile.Load();
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

        // Register raw mouse input
        {
            m_Logger.Info("Registering raw mouse input...");
            
            RAWINPUTDEVICE mouse =
            {
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage     = HID_USAGE_GENERIC_MOUSE,
                .hwndTarget  = Core::Pointer(0x0139815C).as<HWND>(),
            };
            RegisterRawInputDevices(&mouse, 1, sizeof(RAWINPUTDEVICE));

            m_Logger.Info("Registered raw mouse input.");
        }

        // Set window proc
        {
            m_Logger.Info("Setting window proc...");

            m_PreviousWindowProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(
                Core::Pointer(0x0139815C).as<HWND>(),
                GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(&FreeCamera::WindowProc)
            ));

            m_Logger.Info("Set window proc. Previous window proc: 0x%p.", m_PreviousWindowProc);
        }

        // Attach ArbitratorUpdate detour.
        {
            m_Logger.Info("Attaching ArbitratorUpdate detour...");
            
            ModManager::Get().GetDetourHookManager().Attach(m_DetourArbitratorUpdate);
            
            m_Logger.Info("Attached ArbitratorUpdate detour.");
        }

        // Attach SetPlayerVehicleIndex detour.
        {
            m_Logger.Info("Attaching SetPlayerVehicleIndex detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourSetPlayerVehicleIndex);

            m_Logger.Info("Attached SetPlayerVehicleIndex detour.");
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

void FreeCamera::Unload()
{
    try
    {
        m_Logger.Info("Unloading...");

        // Save custom parameters.
        {
            m_CustomParamtersFile.Save();
        }

        // Remove menu.
        {
            m_Logger.Info("Removing menu...");

            ModManager::Get().GetImGuiManager().RemoveMenu(&m_Menu);

            m_Logger.Info("Removed menu.");
        }

        // Detach SetPlayerVehicleIndex detour.
        {
            m_Logger.Info("Detaching SetPlayerVehicleIndex detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourSetPlayerVehicleIndex);

            m_Logger.Info("Detached SetPlayerVehicleIndex detour.");
        }

        // Detach ArbitratorUpdate detour.
        {
            m_Logger.Info("Detaching ArbitratorUpdate detour...");
            
            ModManager::Get().GetDetourHookManager().Detach(m_DetourArbitratorUpdate);
            
            m_Logger.Info("Detached ArbitratorUpdate detour.");
        }

        // Set previous window proc
        {
            m_Logger.Info("Setting previous window proc...");

            SetWindowLongPtrA(
                Core::Pointer(0x0139815C).as<HWND>(),
                GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(m_PreviousWindowProc)
            );

            m_Logger.Info("Set previous window proc.");
        }
        
        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
    }
}

void FreeCamera::OnArbitratorUpdate(void* camera, void* arbStateSharedInfo)
{
    m_GameplayExternalCamera.OnArbitratorUpdate(camera, arbStateSharedInfo);
    m_Behaviors.OnArbitratorUpdate(camera, arbStateSharedInfo);
    m_CurrentCamera.OnArbitratorUpdate(camera, arbStateSharedInfo);
}

void FreeCamera::OnSetPlayerVehicleIndex(void* directorInputBuffer)
{
    m_Misc.OnSetPlayerVehicleIndex(directorInputBuffer);
}

void FreeCamera::OnRenderMenu()
{
    if (ImGui::Begin(k_ModName, nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.0f);
        
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_ModVersion);
        ImGui::Text("Author      %s", k_ModAuthor);
        ImGui::Text("Framerate   %.3f", io.Framerate);

        m_Misc.OnRenderMenu();
        m_CurrentCamera.OnRenderMenu();
        m_GameplayExternalCamera.OnRenderMenu();
        m_Behaviors.OnRenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}

LRESULT CALLBACK FreeCamera::WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_INPUT:
        if (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT)
        {
            RAWINPUT rawInput = {};
            UINT size = sizeof(rawInput);
            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &rawInput, &size, sizeof(RAWINPUTHEADER));
            
            if (rawInput.header.dwType == RIM_TYPEMOUSE)
            {
                s_Instance.m_CurrentCamera.OnMouseInput(rawInput.data.mouse);
            }
        }
        break;
    }
    
    return CallWindowProcA(s_Instance.m_PreviousWindowProc, hWnd, Msg, wParam, lParam);
}

__declspec(naked) void FreeCamera::DetourArbitratorUpdate()
{
    __asm
    {
        pushfd
        pushad

        push dword ptr [ebp + 0x10] // BrnDirector::ArbStateSharedInfo*
        push dword ptr [ebp + 0xC]  // BrnDirector::Camera::Camera*
        mov ecx, offset s_Instance
        call FreeCamera::OnArbitratorUpdate

        popad
        popfd
        
        jmp dword ptr [s_Instance.m_DetourArbitratorUpdate.Target]
    }
}

__declspec(naked) void FreeCamera::DetourSetPlayerVehicleIndex()
{
    __asm
    {
        pushfd
        pushad
        
        push edi // BrnDirector::DirectorIO::InputBuffer*
        mov ecx, offset s_Instance
        call FreeCamera::OnSetPlayerVehicleIndex

        popad
        popfd

        jmp dword ptr [s_Instance.m_DetourSetPlayerVehicleIndex.Target]
    }
}
