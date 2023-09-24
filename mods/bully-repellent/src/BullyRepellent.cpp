#include "BullyRepellent.h"

#include "imgui/imgui.h"

#include "core/Pointer.h"

#include "mod-manager/ModManager.h"


using namespace std::string_literals;


static constexpr char k_ModName[]      = "Bully Repellent";
static constexpr char k_ModVersion[]   = "1.0.0";
static constexpr char k_ModAuthor[]    = "PISros0724 (Matty)";
static constexpr char k_ModDirectory[] = ".\\mods\\bully-repellent\\";


extern BullyRepellent* g_Mod;


BullyRepellent::BullyRepellent(HMODULE module)
    :
    Mod(module),
    m_Logger(k_ModName),
    m_CurrentLobby(m_Logger, k_ModDirectory + "blacklisted-players.yaml"s),
    m_DetourUpatePlayerStatus
    {
        .HookAddress    = Core::Pointer(0x0092BF4D).GetAddress(),
        .DetourFunction = &BullyRepellent::DetourUpatePlayerStatus,
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

void BullyRepellent::Load()
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

        // Attach UpatePlayerStatus detour.
        {
            m_Logger.Info("Attaching UpatePlayerStatus detour...");

            ModManager::Get().GetDetourHookManager().AttachDetourHook(m_DetourUpatePlayerStatus);

            m_Logger.Info("Attached UpatePlayerStatus detour.");
        }

        // Add menu.
        {
            m_Logger.Info("Adding menu...");

            ModManager::Get().GetImGuiManager().AddMenu(&m_Menu);

            m_Logger.Info("Added menu.");
        }

        // Load blacklisted players.
        {
            m_CurrentLobby.LoadBlacklistedPlayers();
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

        // Save blacklisted players.
        {
            m_CurrentLobby.SaveBlacklistedPlayers();
        }

        // Detach UpatePlayerStatus detour.
        {
            m_Logger.Info("Detaching UpatePlayerStatus detour...");

            ModManager::Get().GetDetourHookManager().DetachDetourHook(m_DetourUpatePlayerStatus);

            m_Logger.Info("Detached UpatePlayerStatus detour.");
        }

        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(nullptr, e.what(), k_ModName, MB_ICONERROR);
    }
}

void BullyRepellent::OnUpdate(void* guiCache)
{
    m_CurrentLobby.OnUpdate(guiCache);
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

__declspec(naked) void BullyRepellent::DetourUpatePlayerStatus()
{
    __asm
    {
        pushfd
        pushad

        push ebx // GuiCache*
        mov ecx, dword ptr [g_Mod]
        call BullyRepellent::OnUpdate

        popad
        popfd
        ret
    }
}