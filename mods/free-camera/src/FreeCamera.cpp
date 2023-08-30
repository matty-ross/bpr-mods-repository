#include "FreeCamera.h"

#include "core/Pointer.h"

#include "mod-manager/ModManager.h"

#include "imgui/imgui.h"


FreeCamera::FreeCamera(HMODULE module)
    :
    Mod(module),
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

    ModManager::Get().GetImGuiManager().AddMenu(&m_Menu);
}

void FreeCamera::Unload()
{
}

void FreeCamera::OnRenderMenu()
{
    if (ImGui::Begin("Free Camera"))
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGui::Text("Framerate: %.3f", io.Framerate);
    }
    ImGui::End();
}