#pragma once


#include "VehiclesFile.h"


class VehicleProtection
{
public:
    VehicleProtection(VehiclesFile& vehiclesFile);

public:
    void OnRenderMenu();

private:
    VehiclesFile& m_VehiclesFile;

    bool m_VehicleProtectionEnabled = true;
};