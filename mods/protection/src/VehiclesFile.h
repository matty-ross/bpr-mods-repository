#pragma once


#include <string>
#include <map>
#include <vector>

#include "core/Logger.h"


class VehiclesFile
{
public:
    VehiclesFile(const Core::Logger& logger, const std::string& filePath);

public:
    void Load();
    void Save();

    uint64_t GetFallbackVehicleID() const;
    void SetFallbackVehicleID(uint64_t fallbackVehicleID);

    std::map<uint64_t, uint64_t>& GetVehicles();
    const std::vector<uint64_t>& GetVehicleIDs() const;
    void AddVehicle(uint64_t vehicleID, uint64_t vehicleReplacementID);

private:
    const Core::Logger& m_Logger;

    std::string m_FilePath;
    
    uint64_t m_FallbackVehicleID = 0x0000000000000000;

    std::map<uint64_t, uint64_t> m_Vehicles;
    std::vector<uint64_t> m_VehicleIDs;
};