#pragma once


#include <Windows.h>

#include "core/Logger.hpp"
#include "mod-manager/DetourHookManager.hpp"
#include "mod-manager/ImGuiManager.hpp"

#include "Environment.hpp"
#include "VehicleManager.hpp"


class ModMenu
{
private:
    ModMenu();

public:
    static ModMenu& Get();

public:
    void OnProcessAttach();
    void OnProcessDetach();

private:
    void Load();
    void Unload();

    void OnPreWorldUpdate(void* gameEventQueue, void* gameActionQueue);
    void OnUpdateActiveRaceVehicleColors();

    void OnRenderMenu();

private:
    static void DetourPreWorldUpdate();
    static void DetourUpdateActiveRaceVehicleColors();

private:
    static ModMenu s_Instance;

private:
    Core::Logger m_Logger;

    Environment m_Environment;
    VehicleManager m_VehicleManager;

    DetourHook m_DetourPreWorldUpdate;
    DetourHook m_DetourUpdateActiveRaceVehicleColors;

    ImGuiMenu m_Menu;

    HANDLE m_LoadThreadHandle = NULL;
};
