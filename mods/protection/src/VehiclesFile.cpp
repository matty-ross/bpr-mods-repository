#include "VehiclesFile.h"

#include <algorithm>

#include <Windows.h>

#include "yaml-cpp/yaml.h"

#include "core/File.h"


VehiclesFile::VehiclesFile(const std::string& filePath, const Core::Logger& logger)
    :
    m_FilePath(filePath),
    m_Logger(logger)
{
}

void VehiclesFile::Load()
{
    auto readFile = [this]() -> std::string
    {
        try
        {
            Core::File file(m_FilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_ALWAYS);
            return file.Read();
        }
        catch (const std::runtime_error& e)
        {
            m_Logger.Warning("%s. Last error: 0x%08X.", e.what(), GetLastError());
        }

        return std::string();
    };

    m_Valid = false;
    
    try
    {
        m_Logger.Info("Loading vehicles from file '%s' ...", m_FilePath.c_str());

        m_Vehicles.clear();
        
        YAML::Node yaml = YAML::Load(readFile());
        {
            uint64_t fallbackVehicleID = yaml["FallbackID"].as<uint64_t>();
            const VanillaVehicle* fallbackVehicle = GetVanillaVehicle(fallbackVehicleID);
            m_FallbackVehicle = fallbackVehicle != nullptr ? fallbackVehicle : k_LastResortFallbackVehicle;
        }
        for (const YAML::Node& vehicleNode : yaml["Vehicles"])
        {
            uint64_t replacementVehicleID = vehicleNode["ReplacementID"].as<uint64_t>();
            const VanillaVehicle* replacementVehicle = GetVanillaVehicle(replacementVehicleID);
            Vehicle vehicle =
            {
                .ID          = vehicleNode["ID"].as<uint64_t>(),
                .Name        = vehicleNode["Name"].as<std::string>(),
                .Replacement = replacementVehicle != nullptr ? replacementVehicle : m_FallbackVehicle,
            };

            m_Vehicles.push_back(vehicle);
        }

        m_Logger.Info("Loaded vehicles.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load vehicles. %s", e.what());
    }
    
    m_Valid = true;
}

void VehiclesFile::Save() const
{
    auto writeFile = [this](const std::string& content) -> void
    {
        try
        {
            Core::File file(m_FilePath, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS);
            file.Write(content);
        }
        catch (const std::runtime_error& e)
        {
            m_Logger.Warning("%s. Last error: 0x%08X.", e.what(), GetLastError());
        }
    };

    if (!m_Valid)
    {
        return;
    }
    
    try
    {
        m_Logger.Info("Saving vehicles to file '%s' ...", m_FilePath.c_str());

        YAML::Node yaml;
        {
            yaml["FallbackID"] = m_FallbackVehicle->ID;
        }
        for (const Vehicle& vehicle : m_Vehicles)
        {
            YAML::Node vehicleNode;
            vehicleNode["ID"]            = vehicle.ID;
            vehicleNode["Name"]          = vehicle.Name;
            vehicleNode["ReplacementID"] = vehicle.Replacement->ID;
            
            yaml["Vehicles"].push_back(vehicleNode);
        }
        writeFile(YAML::Dump(yaml));

        m_Logger.Info("Saved vehicles.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save vehicles. %s", e.what());
    }
}

std::vector<Vehicle>& VehiclesFile::GetVehicles()
{
    return m_Vehicles;
}

Vehicle* VehiclesFile::GetVehicle(uint64_t vehicleID)
{
    auto it = std::find_if(m_Vehicles.begin(), m_Vehicles.end(),
        [=](const Vehicle& vehicle)
        {
            return vehicle.ID == vehicleID;
        }
    );
    return it != m_Vehicles.end() ? &(*it) : nullptr;
}

const VanillaVehicle* VehiclesFile::GetFallbackVehicle() const
{
    return m_FallbackVehicle;
}

void VehiclesFile::SetFallbackVehicle(const VanillaVehicle* fallbackVehicle)
{
    m_FallbackVehicle = fallbackVehicle;
}