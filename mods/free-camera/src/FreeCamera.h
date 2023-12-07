#pragma once


#include <Windows.h>

#include "core/Mod.h"
#include "core/Logger.h"

#include "mod-manager/DetourHookManager.h"
#include "mod-manager/ImGuiManager.h"

#include "CustomParametersFile.h"
#include "Misc.h"
#include "CurrentCamera.h"
#include "GameplayExternalCamera.h"
#include "Behaviors.h"


class FreeCamera : public Core::Mod
{
public:
    FreeCamera(HMODULE module);

private:
    void Load() override;
    void Unload() override;

    void OnUpdate(void* camera, void* sharedInfo);
    void OnRenderMenu();

private:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
    static void DetourArbitratorUpdate();

private:
    Core::Logger m_Logger;

    WNDPROC m_PreviousWindowProc = nullptr;
    
    CustomParametersFile m_CustomParamtersFile;
    
    Misc m_Misc;
    CurrentCamera m_CurrentCamera;
    GameplayExternalCamera m_GameplayExternalCamera;
    Behaviors m_Behaviors;

    DetourHook m_DetourArbitratorUpdate;
    
    ImGuiMenu m_Menu;
};