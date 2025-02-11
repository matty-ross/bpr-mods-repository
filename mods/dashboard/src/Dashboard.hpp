#pragma once


#include <Windows.h>

#include "core/Logger.hpp"
#include "mod-manager/DetourHookManager.hpp"
#include "mod-manager/ImGuiManager.hpp"

#include "DashboardConfigFile.hpp"
#include "DashboardHud.hpp"


class Dashboard
{
private:
    Dashboard();

public:
    static Dashboard& Get();

public:
    void OnProcessAttach();
    void OnProcessDetach();

private:
    void Load();
    void Unload();

    void OnProgressionAddDistanceDriven(void* progressionManager, float distance, int32_t vehicleType);

    void OnRenderMenu();
    void OnRenderOverlay();

private:
    static void DetourProgressionAddDistanceDriven();

private:
    static Dashboard s_Instance;

private:
    Core::Logger m_Logger;

    DashboardConfigFile m_DashboardConfigFile;

    DashboardHud m_DashboardHud;

    DetourHook m_DetourProgressionAddDistanceDriven;

    ImGuiMenu m_Menu;
    ImGuiOverlay m_Overlay;

    HANDLE m_LoadThreadHandle = NULL;
};
