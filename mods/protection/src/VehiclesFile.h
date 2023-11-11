#pragma once


#include <string>
#include <map>
#include <vector>

#include "core/Logger.h"

#include "Vehicles.h"


struct Vehicle
{
    VehicleID NewID;
    const VehicleID* ReplacementID;
};


class VehiclesFile
{
public:
    VehiclesFile(const Core::Logger& logger, const std::string& filePath);

public:
    void Load();
    void Save() const;

    const std::vector<uint64_t>& GetVehicleIDs() const;
    Vehicle* GetVehicle(uint64_t vehicleID);
    void AddVehicle(const Vehicle& vehicle);

    const VehicleID* GetFallbackVehicleID() const;
    void SetFallbackVehicleID(const VehicleID* fallbackVehicleID);

private:
    const Core::Logger& m_Logger;

    std::string m_FilePath;

    std::map<uint64_t, Vehicle> m_Vehicles;
    std::vector<uint64_t> m_VehicleIDs;
    
    const VehicleID* m_FallbackVehicleID = k_LastResortFallbackVehicleID;

    bool m_Valid = false;
};