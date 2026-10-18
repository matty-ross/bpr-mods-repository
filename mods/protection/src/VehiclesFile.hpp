#pragma once


#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include <core/Path.hpp>
#include <core/Logger.hpp>

#include "VanillaVehicles.hpp"


class VehiclesFile
{
public:
    struct Vehicle
    {
        uint64_t ID = 0;
        std::string Name;
        const VanillaVehicle* Replacement = nullptr;
    };

public:
    VehiclesFile(Core::Path configDirectoryPath, const Core::Logger& logger);

public:
    std::span<Vehicle> GetVehicles();
    Vehicle* FindVehicleByID(uint64_t vehicleID);
    void AddVehicle(const Vehicle& vehicle);

    const VanillaVehicle* GetFallbackVehicle() const;
    void SetFallbackVehicle(const VanillaVehicle* fallbackVehicle);

    bool GetReplaceLocalPlayerVehicle() const;
    void SetReplaceLocalPlayerVehicle(bool replaceLocalPlayerVehicle);

    void Load();
    void Save() const;

private:
    static constexpr char k_Name[] = "vehicles";

private:
    Core::Path m_FilePath;

    std::vector<Vehicle> m_Vehicles;
    const VanillaVehicle* m_FallbackVehicle = k_LastResortFallbackVehicle;
    bool m_ReplaceLocalPlayerVehicle = false;

    const Core::Logger& m_Logger;
};
