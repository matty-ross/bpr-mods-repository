#pragma once


#include <Windows.h>

#include "core/Logger.hpp"
#include "mod-manager/DetourHookManager.hpp"
#include "mod-manager/ImGuiManager.hpp"


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

    void OnProgressionAddDistanceDriven(float distance, int32_t vehicleType);

    void OnRenderOverlay();

private:
    static void DetourProgressionAddDistanceDriven();

private:
    static Dashboard s_Instance;

private:
    Core::Logger m_Logger;

    DetourHook m_DetourProgressionAddDistanceDriven;

    ImGuiOverlay m_Overlay;

    HANDLE m_LoadThreadHandle = NULL;
};
