#pragma once


#include <Windows.h>

#include "core/Logger.hpp"
#include "mod-manager/DetourHookManager.hpp"
#include "mod-manager/ImGuiManager.hpp"

#include "Misc.hpp"
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

    void OnDoUpdate();
    void OnPreWorldUpdate(void* gameEventQueue, void* gameActionQueue);
    void OnUpdateHudFlow();
    void OnUpdateActiveRaceVehicleColors();

    void OnRenderMenu();

private:
    static void DetourDoUpdate();
    static void DetourPreWorldUpdate();
    static void DetourUpdateHudFlow();
    static void DetourUpdateActiveRaceVehicleColors();
    static void DetourCheckSwitchableBoost();

private:
    static ModMenu s_Instance;

private:
    Core::Logger m_Logger;

    Misc m_Misc;
    Environment m_Environment;
    VehicleManager m_VehicleManager;

    DetourHook m_DetourDoUpdate;
    DetourHook m_DetourPreWorldUpdate;
    DetourHook m_DetourUpdateHudFlow;
    DetourHook m_DetourUpdateActiveRaceVehicleColors;
    DetourHook m_DetourCheckSwitchableBoost;

    ImGuiMenu m_Menu;

    HANDLE m_LoadThreadHandle = NULL;
};
