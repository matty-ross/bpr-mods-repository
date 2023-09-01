#pragma once


#include <Windows.h>

#include "core/Mod.h"

#include "mod-manager/DetourHook.h"
#include "mod-manager/ImGuiManager.h"

#include "GameplayExternalCamera.h"


class FreeCamera : public Core::Mod
{
public:
    FreeCamera(HMODULE module);

public:
    void OnProcessAttach() override;
    void OnProcessDetach() override;
    void OnThreadAttach() override;
    void OnThreadDetach() override;

private:
    void Load();
    void Unload();

    void OnUpdate(void* camera, void* sharedInfo);
    void OnRenderMenu();

private:
    static void DetourArbitratorUpdate();

private:
    HANDLE m_LoadThread = nullptr;

    DetourHook m_DetourArbitratorUpdate;
    ImGuiMenu m_Menu;

    GameplayExternalCamera m_GameplayExternalCamera;
};