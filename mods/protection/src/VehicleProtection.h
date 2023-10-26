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

    void OnPlayerParamsSerialize(void* playerParams);
    void OnPlayerParamsDeserialize(void* playerParams);

    uint64_t HandleVehicleID(uint64_t vehicleID);
    
    void AddNonVanillaVehicleIDsToVehiclesFile();
    void ValidateReplacementVehicles();

private:
    VehiclesFile& m_VehiclesFile;

    bool m_VehicleProtectionEnabled = true;
};