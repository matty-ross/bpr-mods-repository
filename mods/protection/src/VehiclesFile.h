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
    void Save();

    std::map<uint64_t, Vehicle>& GetVehicles();
    const std::vector<uint64_t>& GetVehicleIDs() const;
    void AddVehicle(const Vehicle& vehicle);

private:
    const Core::Logger& m_Logger;

    std::string m_FilePath;

    std::map<uint64_t, Vehicle> m_Vehicles;
    std::vector<uint64_t> m_VehicleIDs;
};