#pragma once


#include <string>
#include <vector>

#include "core/Logger.hpp"

#include "Vehicles.hpp"


struct Vehicle
{
    uint64_t ID = 0;
    std::string Name;
    const VanillaVehicle* Replacement = nullptr;
};


class VehiclesFile
{
public:
    VehiclesFile(const char* filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    std::vector<Vehicle>& GetVehicles();
    Vehicle* GetVehicle(uint64_t vehicleID);
    const VanillaVehicle* GetFallbackVehicle() const;
    void SetFallbackVehicle(const VanillaVehicle* fallbackVehicle);

private:
    const char* m_FilePath;
    const Core::Logger& m_Logger;

    std::vector<Vehicle> m_Vehicles;
    const VanillaVehicle* m_FallbackVehicle = k_LastResortFallbackVehicle;
};
