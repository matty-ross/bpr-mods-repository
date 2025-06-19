#include "ModMenu.hpp"

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "mod-manager/ModManager.hpp"


#define MOD_DIRECTORY ".\\mods\\mod-menu\\"


static constexpr char k_ModName[] = "Mod Menu";
static constexpr char k_ModVersion[] = "1.2.0";
static constexpr char k_ModAuthor[] = "PISros0724 (Matty)";


ModMenu ModMenu::s_Instance;


ModMenu::ModMenu()
    :
    m_Logger(k_ModName),
    m_DetourPreWorldUpdate
    {
        .Target = Core::Pointer(0x00A2A512).GetAddress(),
        .Detour = &ModMenu::DetourPreWorldUpdate,
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
        static_cast<ModMenu*>(lpThreadParameter)->Load();
        return 0;
    };
    m_LoadThreadHandle = CreateThread(nullptr, 0, loadThreadProc, this, 0, nullptr);
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

        // Check versions.
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
            m_Logger.Info("Creating mod directory '%s' ...", MOD_DIRECTORY);

            CreateDirectoryA(MOD_DIRECTORY, nullptr);

            m_Logger.Info("Created mod directory.");
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

        // Attach PreWorldUpdate detour.
        {
            m_Logger.Info("Attaching PreWorldUpdate detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourPreWorldUpdate);

            m_Logger.Info("Attached PreWorldUpdate detour.");
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

        // Remove menu.
        {
            m_Logger.Info("Removing menu...");

            ModManager::Get().GetImGuiManager().RemoveMenu(&m_Menu);

            m_Logger.Info("Removed menu.");
        }

        // Detach PreWorldUpdate detour.
        {
            m_Logger.Info("Detaching PreWorldUpdate detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourPreWorldUpdate);

            m_Logger.Info("Detached PreWorldUpdate detour.");
        }

        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
    }
}

void ModMenu::OnPreWorldUpdate(void* gameEventQueue, void* gameActionQueue)
{
    m_Vehicle.OnPreWorldUpdate(gameEventQueue, gameActionQueue);
}

void ModMenu::OnRenderMenu()
{
    if (ImGui::Begin(k_ModName, nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.0f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_ModVersion);
        ImGui::Text("Author      %s", k_ModAuthor);
        ImGui::Text("Framerate   %.3f", io.Framerate);

        m_Environment.OnRenderMenu();
        m_Vehicle.OnRenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
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
