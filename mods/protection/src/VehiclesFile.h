#pragma once


#include <string>
#include <map>
#include <vector>

#include "core/Logger.h"

#include "Vehicles.h"


struct Vehicle
{
    std::string Name;
    const VanillaVehicle* Replacement = nullptr;
};


class VehiclesFile
{
public:
    VehiclesFile(const std::string& filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    const std::vector<uint64_t>& GetVehicleIDs() const;
    Vehicle* GetVehicle(uint64_t vehicleID);
    void AddVehicle(uint64_t vehicleID, const Vehicle& vehicle);

    const VanillaVehicle* GetFallbackVehicle() const;
    void SetFallbackVehicle(const VanillaVehicle* fallbackVehicle);

private:
    std::string m_FilePath;
    const Core::Logger& m_Logger;

    std::map<uint64_t, Vehicle> m_Vehicles;
    std::vector<uint64_t> m_VehicleIDs;
    
    const VanillaVehicle* m_FallbackVehicle = k_LastResortFallbackVehicle;

    bool m_Valid = false;
};