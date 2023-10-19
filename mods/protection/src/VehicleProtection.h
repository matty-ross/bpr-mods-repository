#pragma once


#include <vector>

#include "Vehicles.h"
#include "VehiclesFile.h"


class VehicleProtection
{
public:
    VehicleProtection(VehiclesFile& vehiclesFile);

public:
    void OnRenderMenu();

    void InitNonVanillaVehicleIDs();
    const std::vector<VehicleID>& GetNonVanillaVehicleIDs() const;

private:
    VehiclesFile& m_VehiclesFile;
    
    std::vector<VehicleID> m_NonVanillaVehicleIDs;

    bool m_VehicleProtectionEnabled = true;
};