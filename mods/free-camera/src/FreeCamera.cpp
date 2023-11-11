#include "FreeCamera.h"

#include "imgui/imgui.h"

#include "core/Pointer.h"

#include "mod-manager/ModManager.h"


using namespace std::string_literals;


static constexpr char k_ModName[]      = "Free Camera";
static constexpr char k_ModVersion[]   = "1.0.0";
static constexpr char k_ModAuthor[]    = "PISros0724 (Matty)";
static constexpr char k_ModDirectory[] = ".\\mods\\free-camera\\";


extern FreeCamera* g_Mod;


FreeCamera::FreeCamera(HMODULE module)
    :
    Mod(module),
    m_Logger(k_ModName),
    m_CustomParamtersFile(m_Logger, k_ModDirectory + "custom-parameters.yaml"s),
    m_GameplayExternalCamera(m_CustomParamtersFile),
    m_DetourArbitratorUpdate
    {
        .HookAddress    = Core::Pointer(0x009645E0).GetAddress(),
        .DetourFunction = &FreeCamera::DetourArbitratorUpdate,
        .StubFunction   = nullptr,
    },
    m_Menu
    {
        .OnRenderMenuFunction   = [this]() { OnRenderMenu(); },
        .ToggleVisibilityHotkey = VK_F7,
        .Visible                = true,
    }
{
}

void FreeCamera::Load()
{
    try
    {
        m_Logger.Info("Loading...");

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

        // Attach ArbitratorUpdate detour.
        {
            m_Logger.Info("Attaching ArbitratorUpdate detour...");
            
            ModManager::Get().GetDetourHookManager().AttachDetourHook(m_DetourArbitratorUpdate);
            
            m_Logger.Info("Attached ArbitratorUpdate detour.");
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

        // Detach ArbitratorUpdate detour.
        {
            m_Logger.Info("Detaching ArbitratorUpdate detour...");
            
            ModManager::Get().GetDetourHookManager().DetachDetourHook(m_DetourArbitratorUpdate);
            
            m_Logger.Info("Detached ArbitratorUpdate detour.");
        }
        
        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(nullptr, e.what(), k_ModName, MB_ICONERROR);
    }
}

void FreeCamera::OnUpdate(void* camera, void* sharedInfo)
{
    m_CurrentCamera.OnUpdate(camera, sharedInfo);
    m_GameplayExternalCamera.OnUpdate(camera, sharedInfo);
    m_Behaviors.OnUpdate(camera, sharedInfo);
}

void FreeCamera::OnRenderMenu()
{
    if (ImGui::Begin(k_ModName))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);
        
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_ModVersion);
        ImGui::Text("Author      %s", k_ModAuthor);
        ImGui::Text("Framerate   %.3f", io.Framerate);

        m_CurrentCamera.OnRenderMenu();
        m_GameplayExternalCamera.OnRenderMenu();
        m_Behaviors.OnRenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}

__declspec(naked) void FreeCamera::DetourArbitratorUpdate()
{
    __asm
    {
        pushfd
        pushad

        push dword ptr [ebp + 0x10] // BrnDirector::ArbStateSharedInfo*
        push dword ptr [ebp + 0xC]  // BrnDirector::Camera::Camera*
        mov ecx, dword ptr [g_Mod]
        call FreeCamera::OnUpdate

        popad
        popfd
        ret
    }
}