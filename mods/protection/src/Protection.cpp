#include "Protection.hpp"

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "mod-manager/ModManager.hpp"


#define MOD_DIRECTORY ".\\mods\\protection\\"


static constexpr char k_ModName[]    = "Protection";
static constexpr char k_ModVersion[] = "1.2.0";
static constexpr char k_ModAuthor[]  = "PISros0724 (Matty)";


Protection Protection::s_Instance;


Protection::Protection()
    :    
    m_Logger(k_ModName),
    m_VehiclesFile(MOD_DIRECTORY "vehicles.yaml", m_Logger),
    m_ChallengesFile(MOD_DIRECTORY "challenges.yaml", m_Logger),
    m_VehicleProtection(m_VehiclesFile),
    m_ChallengeProtection(m_ChallengesFile),
    m_DetourPlayerParamsSerialize
    {
        .Target = Core::Pointer(0x00B7218A).GetAddress(),
        .Detour = &Protection::DetourPlayerParamsSerialize,
    },
    m_DetourPlayerParamsDeserialize
    {
        .Target = Core::Pointer(0x00B72958).GetAddress(),
        .Detour = &Protection::DetourPlayerParamsDeserialize,
    },
    m_DetourVehicleSelectMessagePack
    {
        .Target = Core::Pointer(0x00B62095).GetAddress(),
        .Detour = &Protection::DetourVehicleSelectMessagePack,
    },
    m_DetourVehicleSelectMessageUnpack
    {
        .Target = Core::Pointer(0x00B6209F).GetAddress(),
        .Detour = &Protection::DetourVehicleSelectMessageUnpack,
    },
    m_DetourFreeburnChallengeMessagePack
    {
        .Target = Core::Pointer(0x0790A490).GetAddress(),
        .Detour = &Protection::DetourFreeburnChallengeMessagePack,
    },
    m_DetourFreeburnChallengeMessageUnpack
    {
        .Target = Core::Pointer(0x0790A49A).GetAddress(),
        .Detour = &Protection::DetourFreeburnChallengeMessageUnpack,
    },
    m_Menu
    {
        .OnRenderFunction = [&]() { OnRenderMenu(); },
    }
{
}

Protection& Protection::Get()
{
    return s_Instance;
}

void Protection::OnProcessAttach()
{
    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpThreadParameter) -> DWORD
    {
        s_Instance.Load();
        return 0;
    };
    m_LoadThreadHandle = CreateThread(nullptr, 0, loadThreadProc, nullptr, 0, nullptr);
}

void Protection::OnProcessDetach()
{
    Unload();
    CloseHandle(m_LoadThreadHandle);
}

void Protection::Load()
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

        // Load vehicles.
        {
            m_VehiclesFile.Load();
        }

        // Load challenges.
        {
            m_ChallengesFile.Load();
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

        // Add non-vanilla vehicles.
        {
            m_Logger.Info("Adding non-vanilla vehicles...");

            m_VehicleProtection.AddNonVanillaVehiclesToVehiclesFile();

            m_Logger.Info("Added non-vanilla vehicles.");
        }

        // Add non-vanilla challenges.
        {
            m_Logger.Info("Adding non-vanilla challenges...");

            m_ChallengeProtection.AddNonVanillaChallengesToChallengesFile();

            m_Logger.Info("Added non-vanilla challenges.");
        }

        // Attach PlayerParamsSerialize detour.
        {
            m_Logger.Info("Attaching PlayerParamsSerialize detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourPlayerParamsSerialize);

            m_Logger.Info("Attached PlayerParamsSerialize detour.");
        }

        // Attach PlayerParamsDeserialize detour.
        {
            m_Logger.Info("Attaching PlayerParamsDeserialize detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourPlayerParamsDeserialize);

            m_Logger.Info("Attached PlayerParamsDeserialize detour.");
        }

        // Attach VehicleSelectMessagePack detour.
        {
            m_Logger.Info("Attaching VehicleSelectMessagePack detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourVehicleSelectMessagePack);

            m_Logger.Info("Attached VehicleSelectMessagePack detour.");
        }

        // Attach VehicleSelectMessageUnpack detour.
        {
            m_Logger.Info("Attaching VehicleSelectMessageUnpack detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourVehicleSelectMessageUnpack);

            m_Logger.Info("Attached VehicleSelectMessageUnpack detour.");
        }

        // Attach FreeburnChallengeMessagePack detour.
        {
            m_Logger.Info("Attaching FreeburnChallengeMessagePack detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourFreeburnChallengeMessagePack);

            m_Logger.Info("Attached FreeburnChallengeMessagePack detour.");
        }

        // Attach FreeburnChallengeMessageUnpack detour.
        {
            m_Logger.Info("Attaching FreeburnChallengeMessageUnpack detour...");

            ModManager::Get().GetDetourHookManager().Attach(m_DetourFreeburnChallengeMessageUnpack);

            m_Logger.Info("Attached FreeburnChallengeMessageUnpack detour.");
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

void Protection::Unload()
{
    try
    {
        m_Logger.Info("Unloading...");

        // Save vehicles.
        {
            m_VehiclesFile.Save();
        }

        // Save challenges.
        {
            m_ChallengesFile.Save();
        }

        // Remove menu.
        {
            m_Logger.Info("Removing menu...");

            ModManager::Get().GetImGuiManager().RemoveMenu(&m_Menu);

            m_Logger.Info("Removed menu.");
        }

        // Detach FreeburnChallengeMessageUnpack detour.
        {
            m_Logger.Info("Detaching FreeburnChallengeMessageUnpack detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourFreeburnChallengeMessageUnpack);

            m_Logger.Info("Detached FreeburnChallengeMessageUnpack detour.");
        }

        // Detach FreeburnChallengeMessagePack detour.
        {
            m_Logger.Info("Detaching FreeburnChallengeMessagePack detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourFreeburnChallengeMessagePack);

            m_Logger.Info("Detached FreeburnChallengeMessagePack detour.");
        }

        // Detach VehicleSelectMessageUnpack detour.
        {
            m_Logger.Info("Detaching VehicleSelectMessageUnpack detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourVehicleSelectMessageUnpack);

            m_Logger.Info("Detached VehicleSelectMessageUnpack detour.");
        }

        // Detach VehicleSelectMessagePack detour.
        {
            m_Logger.Info("Detaching VehicleSelectMessagePack detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourVehicleSelectMessagePack);

            m_Logger.Info("Detached VehicleSelectMessagePack detour.");
        }

        // Detach PlayerParamsDeserialize detour.
        {
            m_Logger.Info("Detaching PlayerParamsDeserialize detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourPlayerParamsDeserialize);

            m_Logger.Info("Detached PlayerParamsDeserialize detour.");
        }

        // Detach PlayerParamsSerialize detour.
        {
            m_Logger.Info("Detaching PlayerParamsSerialize detour...");

            ModManager::Get().GetDetourHookManager().Detach(m_DetourPlayerParamsSerialize);

            m_Logger.Info("Detached PlayerParamsSerialize detour.");
        }

        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
    }
}

void Protection::OnPlayerParamsSerialize(void* playerParams)
{
    m_VehicleProtection.OnPlayerParamsSerialize(playerParams);
}

void Protection::OnPlayerParamsDeserialize(void* playerParams)
{
    m_VehicleProtection.OnPlayerParamsDeserialize(playerParams);
}

void Protection::OnVehicleSelectMessagePack(void* vehicleSelectMessage)
{
    m_VehicleProtection.OnVehicleSelectMessagePack(vehicleSelectMessage);
}

void Protection::OnVehicleSelectMessageUnpack(void* vehicleSelectMessage)
{
    m_VehicleProtection.OnVehicleSelectMessageUnpack(vehicleSelectMessage);
}

void Protection::OnFreeburnChallengeMessagePack(void* freeburnChallengeMessage)
{
    m_ChallengeProtection.OnFreeburnChallengeMessagePack(freeburnChallengeMessage);
}

void Protection::OnFreeburnChallengeMessageUnpack(void* freeburnChallengeMessage)
{
    m_ChallengeProtection.OnFreeburnChallengeMessageUnpack(freeburnChallengeMessage);
}

void Protection::OnRenderMenu()
{
    if (ImGui::Begin(k_ModName, nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.0f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_ModVersion);
        ImGui::Text("Author      %s", k_ModAuthor);
        ImGui::Text("Framerate   %.1f", io.Framerate);

        m_VehicleProtection.OnRenderMenu();
        m_ChallengeProtection.OnRenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}

__declspec(naked) void Protection::DetourPlayerParamsSerialize()
{
    __asm
    {
        pushfd
        pushad

        push edi // BrnNetwork::PlayerParams*
        mov ecx, offset s_Instance
        call Protection::OnPlayerParamsSerialize

        popad
        popfd
        
        jmp dword ptr [s_Instance.m_DetourPlayerParamsSerialize.Target]
    }
}

__declspec(naked) void Protection::DetourPlayerParamsDeserialize()
{
    __asm
    {
        pushfd
        pushad

        push edi // BrnNetwork::PlayerParams*
        mov ecx, offset s_Instance
        call Protection::OnPlayerParamsDeserialize

        popad
        popfd
        
        jmp dword ptr [s_Instance.m_DetourPlayerParamsDeserialize.Target]
    }
}

__declspec(naked) void Protection::DetourVehicleSelectMessagePack()
{
    __asm
    {
        pushfd
        pushad

        cmp dword ptr [esi + 0x4], 0 // CgsNetwork::Message::EPackOrUnpack::E_PACK_INTO_BITSTREAM
        jne _continue

        push esi // BrnNetwork::CarSelectMessage*
        mov ecx, offset s_Instance
        call Protection::OnVehicleSelectMessagePack

    _continue:
        popad
        popfd
        
        jmp dword ptr [s_Instance.m_DetourVehicleSelectMessagePack.Target]
    }
}

__declspec(naked) void Protection::DetourVehicleSelectMessageUnpack()
{
    __asm
    {
        pushfd
        pushad

        cmp dword ptr [esi + 0x4], 1 // CgsNetwork::Message::EPackOrUnpack::E_UNPACK_FROM_BITSTREAM
        jne _continue

        push esi // BrnNetwork::CarSelectMessage*
        mov ecx, offset s_Instance
        call Protection::OnVehicleSelectMessageUnpack
        
    _continue:
        popad
        popfd
        
        jmp dword ptr [s_Instance.m_DetourVehicleSelectMessageUnpack.Target]
    }
}

__declspec(naked) void Protection::DetourFreeburnChallengeMessagePack()
{
    __asm
    {
        pushfd
        pushad

        cmp dword ptr [esi + 0x4], 0 // CgsNetwork::Message::EPackOrUnpack::E_PACK_INTO_BITSTREAM
        jne _continue

        push esi // BrnNetwork::FreeburnChallengeMessage*
        mov ecx, offset s_Instance
        call Protection::OnFreeburnChallengeMessagePack

    _continue:
        popad
        popfd
        
        jmp dword ptr [s_Instance.m_DetourFreeburnChallengeMessagePack.Target]
    }
}

__declspec(naked) void Protection::DetourFreeburnChallengeMessageUnpack()
{
    __asm
    {
        pushfd
        pushad

        cmp dword ptr [esi + 0x4], 1 // CgsNetwork::Message::EPackOrUnpack::E_UNPACK_FROM_BITSTREAM
        jne _continue

        push esi // BrnNetwork::FreeburnChallengeMessage*
        mov ecx, offset s_Instance
        call Protection::OnFreeburnChallengeMessageUnpack
        
    _continue:
        popad
        popfd
        
        jmp dword ptr [s_Instance.m_DetourFreeburnChallengeMessageUnpack.Target]
    }
}
