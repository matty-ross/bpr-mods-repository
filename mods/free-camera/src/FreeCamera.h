#pragma once


#include <Windows.h>

#include "core/Mod.h"
#include "core/Logger.h"

#include "mod-manager/DetourHookManager.h"
#include "mod-manager/ImGuiManager.h"

#include "CurrentCamera.h"
#include "GameplayExternalCamera.h"
#include "Behaviors.h"


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
    CurrentCamera m_CurrentCamera;
    GameplayExternalCamera m_GameplayExternalCamera;
    Behaviors m_Behaviors;
    
    Core::Logger m_Logger;

    DetourHook m_DetourArbitratorUpdate;
    ImGuiMenu m_Menu;

    HANDLE m_LoadThread = nullptr;
};