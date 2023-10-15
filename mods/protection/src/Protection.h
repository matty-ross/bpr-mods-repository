#pragma once


#include <Windows.h>

#include "core/Mod.h"

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

private:
    Core::Logger m_Logger;

    VehiclesFile m_VehiclesFile;

    VehicleProtection m_VehicleProtection;

    ImGuiMenu m_Menu;
};