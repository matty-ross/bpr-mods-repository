#include <cstdint>
#include <exception>
#include <span>
#include <string>

#include "vendor/yaml-cpp.hpp"

#include "core/Path.hpp"
#include "core/Logger.hpp"
#include "core/File.hpp"

#include "Vehicles.hpp"
#include "VehiclesFile.hpp"


VehiclesFile::VehiclesFile(Core::Path configDirectoryPath, const Core::Logger& logger)
    :
    m_FilePath(configDirectoryPath.Append("vehicles.yaml")),
    m_Logger(logger)
{
}

std::span<VehiclesFile::Vehicle> VehiclesFile::GetVehicles()
{
    return m_Vehicles;
}

VehiclesFile::Vehicle* VehiclesFile::FindVehicleByID(uint64_t vehicleID)
{
    for (Vehicle& vehicle : m_Vehicles)
    {
        if (vehicle.ID == vehicleID)
        {
            return &vehicle;
        }
    }

    return nullptr;
}

void VehiclesFile::AddVehicle(const Vehicle& vehicle)
{
    m_Vehicles.push_back(vehicle);
}

const VanillaVehicle* VehiclesFile::GetFallbackVehicle() const
{
    return m_FallbackVehicle;
}

void VehiclesFile::SetFallbackVehicle(const VanillaVehicle* fallbackVehicle)
{
    m_FallbackVehicle = fallbackVehicle;
}

void VehiclesFile::Load()
{
    try
    {
        YAML::Node yaml = YAML::Load(
            Core::File(m_FilePath, Core::File::Mode::Read, m_Logger).ReadAsText()
        );

        uint64_t fallbackVehicleID = yaml["FallbackID"].as<uint64_t>();
        const VanillaVehicle* fallbackVehicle = FindVanillaVehicleByID(fallbackVehicleID);
        m_FallbackVehicle = fallbackVehicle != nullptr ? fallbackVehicle : k_LastResortFallbackVehicle;

        m_Vehicles.clear();
        for (YAML::Node vehicleNode : yaml["Vehicles"])
        {
            uint64_t replacementVehicleID = vehicleNode["ReplacementID"].as<uint64_t>();
            const VanillaVehicle* replacementVehicle = FindVanillaVehicleByID(replacementVehicleID);

            Vehicle vehicle =
            {
                .ID = vehicleNode["ID"].as<uint64_t>(),
                .Name = vehicleNode["Name"].as<std::string>(),
                .Replacement = replacementVehicle != nullptr ? replacementVehicle : m_FallbackVehicle,
            };
            m_Vehicles.push_back(vehicle);
        }

        m_Logger.Info("Loaded %s.", k_Name);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Warning("Failed to load %s. exception: %s", k_Name, ex.what());
    }
}

void VehiclesFile::Save() const
{
    try
    {
        YAML::Node yaml;

        yaml["FallbackID"] = m_FallbackVehicle->ID;

        for (const Vehicle& vehicle : m_Vehicles)
        {
            YAML::Node vehicleNode;
            vehicleNode["ID"] = vehicle.ID;
            vehicleNode["Name"] = vehicle.Name;
            vehicleNode["ReplacementID"] = vehicle.Replacement->ID;
            yaml["Vehicles"].push_back(vehicleNode);
        }

        Core::File(m_FilePath, Core::File::Mode::Write, m_Logger).WriteAsText(
            YAML::Dump(yaml)
        );

        m_Logger.Info("Saved %s.", k_Name);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Warning("Failed to save %s. exception: %s", k_Name, ex.what());
    }
}
