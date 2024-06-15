#pragma once


#include "core/Pointer.hpp"

#include "VehiclesFile.hpp"


class VehicleProtection
{
public:
    VehicleProtection(VehiclesFile& vehiclesFile);

public:
    void OnPlayerParamsSerialize(Core::Pointer playerParams);
    void OnPlayerParamsDeserialize(Core::Pointer playerParams);
    void OnVehicleSelectMessagePack(Core::Pointer vehicleSelectMessage);
    void OnVehicleSelectMessageUnpack(Core::Pointer vehicleSelectMessage);
    void OnRenderMenu();

    void AddNonVanillaVehiclesToVehiclesFile();
    
private:
    uint64_t HandleVehicleID(uint64_t vehicleID) const;

private:
    VehiclesFile& m_VehiclesFile;
    bool m_VehicleProtectionEnabled = true;
};
