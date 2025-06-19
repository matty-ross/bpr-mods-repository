#pragma once


#include <Windows.h>

#include "core/Logger.hpp"
#include "mod-manager/DetourHookManager.hpp"
#include "mod-manager/ImGuiManager.hpp"

#include "Environment.hpp"
#include "Vehicle.hpp"


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

    void OnRenderMenu();

private:
    static void DetourPreWorldUpdate();

private:
    static ModMenu s_Instance;

private:
    Core::Logger m_Logger;

    Environment m_Environment;
    Vehicle m_Vehicle;

    DetourHook m_DetourPreWorldUpdate;

    ImGuiMenu m_Menu;

    HANDLE m_LoadThreadHandle = NULL;
};
