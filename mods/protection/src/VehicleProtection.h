#pragma once


#include "VehiclesFile.h"


class VehicleProtection
{
public:
    VehicleProtection(VehiclesFile& vehiclesFile);

public:
    void OnPlayerParamsSerialize(void* playerParams);
    void OnPlayerParamsDeserialize(void* playerParams);
    void OnVehicleSelectMessagePack(void* vehicleSelectMessage);
    void OnVehicleSelectMessageUnpack(void* vehicleSelectMessage);
    void OnRenderMenu();

    void AddNonVanillaVehicleIDsToVehiclesFile();
    
private:
    uint64_t HandleVehicleID(uint64_t vehicleID);

private:
    VehiclesFile& m_VehiclesFile;

    bool m_VehicleProtectionEnabled = true;
};