#pragma once


#include <cstdint>

#include "core/Pointer.hpp"
#include "core/Logger.hpp"

#include "VehiclesFile.hpp"


class VehicleProtection
{
public:
    VehicleProtection(VehiclesFile& vehiclesFile, const Core::Logger& logger);

public:
    void Load();

    void RenderMenu();
    void AddNonVanillaVehiclesToVehiclesFile();

private:
    uint64_t HandleVehicleID(uint64_t vehicleID) const;

    void CheckPlayerParamsBeforeSerializing(Core::Pointer playerParams);
    void CheckPlayerParamsAfterDeserializing(Core::Pointer playerParams);
    void CheckVehicleSelectMessageBeforePacking(Core::Pointer vehicleSelectMessage);
    void CheckVehicleSelectMessageAfterUnpacking(Core::Pointer vehicleSelectMessage);

private:
    static void Hook_CheckPlayerParamsBeforeSerializing();
    static void Hook_CheckPlayerParamsAfterDeserializing();
    static void Hook_CheckVehicleSelectMessageBeforePacking();
    static void Hook_CheckVehicleSelectMessageAfterUnpacking();

private:
    VehiclesFile& m_VehiclesFile;
    bool m_VehicleProtectionEnabled = true;

    const Core::Logger& m_Logger;
};
