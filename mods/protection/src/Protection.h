#pragma once


#include <Windows.h>

#include "core/Mod.h"
#include "core/Logger.h"

#include "mod-manager/DetourHookManager.h"
#include "mod-manager/ImGuiManager.h"

#include "VehiclesFile.h"
#include "VehicleProtection.h"


class Protection : public Core::Mod
{
public:
    Protection(HMODULE module);

private:
    void Load() override;
    void Unload() override;

    void OnRenderMenu();

    void OnPlayerParamsSerialize(void* playerParams);

private:
    static void DetourPlayerParamsSerialize();

private:
    Core::Logger m_Logger;

    VehiclesFile m_VehiclesFile;

    VehicleProtection m_VehicleProtection;

    DetourHook m_DetourPlayerParamsSerialize;

    ImGuiMenu m_Menu;
};