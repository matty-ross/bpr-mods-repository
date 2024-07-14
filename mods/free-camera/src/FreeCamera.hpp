#pragma once


#include <Windows.h>

#include "core/Logger.hpp"
#include "mod-manager/DetourHookManager.hpp"
#include "mod-manager/ImGuiManager.hpp"

#include "CustomParametersFile.hpp"
#include "Misc.hpp"
#include "CurrentCamera.hpp"
#include "GameplayExternalCamera.hpp"
#include "Behaviors.hpp"


class FreeCamera
{
private:
    FreeCamera();

public:
    static FreeCamera& Get();

public:
    void OnProcessAttach();
    void OnProcessDetach();

private:
    void Load();
    void Unload();

    void OnArbitratorUpdate(void* camera, void* arbStateSharedInfo);
    void OnSetPlayerVehicleIndex(void* directorInputBuffer);
    
    void OnRenderMenu();

private:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
    
    static void DetourArbitratorUpdate();
    static void DetourSetPlayerVehicleIndex();

private:
    static FreeCamera s_Instance;

private:
    Core::Logger m_Logger;
    
    CustomParametersFile m_CustomParamtersFile;
    
    Misc m_Misc;
    CurrentCamera m_CurrentCamera;
    GameplayExternalCamera m_GameplayExternalCamera;
    Behaviors m_Behaviors;
    
    DetourHook m_DetourArbitratorUpdate;
    DetourHook m_DetourSetPlayerVehicleIndex;

    ImGuiMenu m_Menu;

    HANDLE m_LoadThread = NULL;
    WNDPROC m_PreviousWindowProc = nullptr;
};
