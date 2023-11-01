#pragma once


#include <Windows.h>

#include "core/Mod.h"
#include "core/Logger.h"

#include "mod-manager/DetourHookManager.h"
#include "mod-manager/ImGuiManager.h"

#include "VehiclesFile.h"
#include "ChallengesFile.h"
#include "VehicleProtection.h"


class Protection : public Core::Mod
{
public:
    Protection(HMODULE module);

private:
    void Load() override;
    void Unload() override;

    void OnRenderMenu();

    VehicleProtection& GetVehicleProtection();

private:
    static void DetourPlayerParamsSerialize();
    static void DetourPlayerParamsDeserialize();
    static void DetourVehicleSelectMessagePack();
    static void DetourVehicleSelectMessageUnpack();

private:
    Core::Logger m_Logger;

    VehiclesFile m_VehiclesFile;
    ChallengesFile m_ChallengesFile;

    VehicleProtection m_VehicleProtection;

    DetourHook m_DetourPlayerParamsSerialize;
    DetourHook m_DetourPlayerParamsDeserialize;
    DetourHook m_DetourVehicleSelectMessagePack;
    DetourHook m_DetourVehicleSelectMessageUnpack;

    ImGuiMenu m_Menu;
};