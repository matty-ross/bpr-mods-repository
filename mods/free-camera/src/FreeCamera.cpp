#include "FreeCamera.h"

#include "core/Pointer.h"

#include "mod-manager/ModManager.h"

#include "imgui/imgui.h"


static constexpr char k_ModName[]      = "Free Camera";
static constexpr char k_ModDirectory[] = ".\\mods\\free-camera\\";
static constexpr char k_ModAuthor[]    = "PISros0724 (Matty)";
static constexpr char k_ModVersion[]   = "3.0.0";


extern FreeCamera* g_Mod;


FreeCamera::FreeCamera(HMODULE module)
    :
    Mod(module),
    m_DetourArbitratorUpdate(Core::Pointer(0x009645E0).GetAddress(), &FreeCamera::DetourArbitratorUpdate),
    m_Menu
    {
        .OnRenderMenuFunction = [this]() { OnRenderMenu(); },
        .Visible = true,
    }
{
}

void FreeCamera::OnProcessAttach()
{
    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpThreadParameter) -> DWORD
    {
        static_cast<FreeCamera*>(lpThreadParameter)->Load();
        return 0;
    };
    m_LoadThread = CreateThread(nullptr, 0, loadThreadProc, this, 0, nullptr);
}

void FreeCamera::OnProcessDetach()
{
    WaitForSingleObject(m_LoadThread, 5000);
    CloseHandle(m_LoadThread);

    Unload();
}

void FreeCamera::OnThreadAttach()
{
}

void FreeCamera::OnThreadDetach()
{
}

void FreeCamera::Load()
{
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

    m_DetourArbitratorUpdate.Attach();
    ModManager::Get().GetImGuiManager().AddMenu(&m_Menu);
}

void FreeCamera::Unload()
{
    m_DetourArbitratorUpdate.Detach();
}

void FreeCamera::OnUpdate(void* camera, void* sharedInfo)
{
    m_GameplayExternalCamera.OnUpdate(camera, sharedInfo);
}

void FreeCamera::OnRenderMenu()
{
    if (ImGui::Begin(k_ModName))
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGui::Text("Author      %s", k_ModAuthor);
        ImGui::Text("Version     %s", k_ModVersion);
        ImGui::Text("Framerate   %.3f", io.Framerate);

        m_GameplayExternalCamera.OnRenderMenu();
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