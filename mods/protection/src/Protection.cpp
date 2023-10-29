#include "Protection.h"

#include "imgui/imgui.h"

#include "core/Pointer.h"

#include "mod-manager/ModManager.h"


using namespace std::string_literals;


static constexpr char k_ModName[]      = "Protection";
static constexpr char k_ModVersion[]   = "1.0.0";
static constexpr char k_ModAuthor[]    = "PISros0724 (Matty)";
static constexpr char k_ModDirectory[] = ".\\mods\\protection\\";


extern Protection* g_Mod;


Protection::Protection(HMODULE module)
    :
    Mod(module),
    m_Logger(k_ModName),
    m_VehiclesFile(m_Logger, k_ModDirectory + "vehicles.yaml"s),
    m_VehicleProtection(m_VehiclesFile),
    m_DetourPlayerParamsSerialize
    {
        .HookAddress    = Core::Pointer(0x00B7218A).GetAddress(),
        .DetourFunction = &Protection::DetourPlayerParamsSerialize,
        .StubFunction   = nullptr,
    },
    m_DetourPlayerParamsDeserialize
    {
        .HookAddress    = Core::Pointer(0x00B72958).GetAddress(),
        .DetourFunction = &Protection::DetourPlayerParamsDeserialize,
        .StubFunction   = nullptr,
    },
    m_DetourVehicleSelectMessagePack
    {
        .HookAddress    = Core::Pointer(0x00B62095).GetAddress(),
        .DetourFunction = &Protection::DetourVehicleSelectMessagePack,
        .StubFunction   = nullptr,
    },
    m_DetourVehicleSelectMessageUnpack
    {
        .HookAddress    = Core::Pointer(0x00B6209F).GetAddress(),
        .DetourFunction = &Protection::DetourVehicleSelectMessageUnpack,
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

void Protection::Load()
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

        // Attach PlayerParamsSerialize detour.
        {
            m_Logger.Info("Attaching PlayerParamsSerialize detour...");

            ModManager::Get().GetDetourHookManager().AttachDetourHook(m_DetourPlayerParamsSerialize);

            m_Logger.Info("Attached PlayerParamsSerialize detour.");
        }

        // Attach PlayerParamsDeserialize detour.
        {
            m_Logger.Info("Attaching PlayerParamsDeserialize detour...");

            ModManager::Get().GetDetourHookManager().AttachDetourHook(m_DetourPlayerParamsDeserialize);

            m_Logger.Info("Attached PlayerParamsDeserialize detour.");
        }

        // Add menu.
        {
            m_Logger.Info("Adding menu...");

            ModManager::Get().GetImGuiManager().AddMenu(&m_Menu);

            m_Logger.Info("Added menu.");
        }

        // Load vehicles.
        {
            m_VehiclesFile.Load();
        }

        // Add non-vanilla vehicles.
        {
            m_Logger.Info("Adding non-vanilla vehicles...");

            m_VehicleProtection.AddNonVanillaVehicleIDsToVehiclesFile();

            m_Logger.Info("Added non-vanilla vehicles.");
        }

        m_Logger.Info("Loaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(nullptr, e.what(), k_ModName, MB_ICONERROR);
    }
}

void Protection::Unload()
{
    try
    {
        m_Logger.Info("Unloading...");

        // Save vehicles.
        {
            m_VehiclesFile.Save();
        }

        // Remove menu.
        {
            m_Logger.Info("Removing menu...");

            ModManager::Get().GetImGuiManager().RemoveMenu(&m_Menu);

            m_Logger.Info("Removed menu.");
        }

        // Detach PlayerParamsDeserialize detour.
        {
            m_Logger.Info("Detaching PlayerParamsDeserialize detour...");

            ModManager::Get().GetDetourHookManager().DetachDetourHook(m_DetourPlayerParamsDeserialize);

            m_Logger.Info("Detached PlayerParamsDeserialize detour.");
        }

        // Detach PlayerParamsSerialize detour.
        {
            m_Logger.Info("Detaching PlayerParamsSerialize detour...");

            ModManager::Get().GetDetourHookManager().DetachDetourHook(m_DetourPlayerParamsSerialize);

            m_Logger.Info("Detached PlayerParamsSerialize detour.");
        }

        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(nullptr, e.what(), k_ModName, MB_ICONERROR);
    }
}

void Protection::OnRenderMenu()
{
    if (ImGui::Begin(k_ModName))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_ModVersion);
        ImGui::Text("Author      %s", k_ModAuthor);
        ImGui::Text("Framerate   %.3f", io.Framerate);

        m_VehicleProtection.OnRenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}

VehicleProtection& Protection::GetVehicleProtection()
{
    return m_VehicleProtection;
}

__declspec(naked) void Protection::DetourPlayerParamsSerialize()
{
    __asm
    {
        pushfd
        pushad

        push edi // BrnNetwork::PlayerParams*
        
        mov ecx, dword ptr [g_Mod]
        call Protection::GetVehicleProtection
        mov ecx, eax
        call VehicleProtection::OnPlayerParamsSerialize

        popad
        popfd    
        ret
    }
}

__declspec(naked) void Protection::DetourPlayerParamsDeserialize()
{
    __asm
    {
        pushfd
        pushad

        push edi // BrnNetwork::PlayerParams*
        
        mov ecx, dword ptr [g_Mod]
        call Protection::GetVehicleProtection
        mov ecx, eax
        call VehicleProtection::OnPlayerParamsDeserialize

        popad
        popfd    
        ret
    }
}

__declspec(naked) void Protection::DetourVehicleSelectMessagePack()
{
    __asm
    {
        pushfd
        pushad

        cmp dword ptr [esi + 0x4], 0 // CgsNetwork::Message::E_PACK_INTO_BITSTREAM
        jne _continue

        push esi // BrnNetwork::CarSelectMessage*
        
        mov ecx, dword ptr [g_Mod]
        call Protection::GetVehicleProtection
        mov ecx, eax
        call VehicleProtection::OnVehicleSelectMessagePack

    _continue:
        popad
        popfd
        ret
    }
}

__declspec(naked) void Protection::DetourVehicleSelectMessageUnpack()
{
    __asm
    {
        pushfd
        pushad

        cmp dword ptr [esi + 0x4], 1 // CgsNetwork::Message::E_UNPACK_FROM_BITSTREAM
        jne _continue

        push esi // BrnNetwork::CarSelectMessage*
        
        mov ecx, dword ptr [g_Mod]
        call Protection::GetVehicleProtection
        mov ecx, eax
        call VehicleProtection::OnVehicleSelectMessageUnpack
        
    _continue:
        popad
        popfd
        ret
    }
}