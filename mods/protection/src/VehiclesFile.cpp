#include "VehiclesFile.h"

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
        m_VehicleIDs.clear();
        
        YAML::Node yaml = YAML::Load(readFile());
        {
            uint64_t fallbackVehicleID = yaml["FallbackID"].as<uint64_t>();
            const VanillaVehicle* fallbackVehicle = GetVanillaVehicle(fallbackVehicleID);
            m_FallbackVehicle = fallbackVehicle != nullptr ? fallbackVehicle : k_LastResortFallbackVehicle;
        }
        for (const YAML::Node& vehicleNode : yaml["Vehicles"])
        {
            uint64_t vehicleID = vehicleNode["ID"].as<uint64_t>();
            uint64_t replacementVehicleID = vehicleNode["ReplacementID"].as<uint64_t>();
            const VanillaVehicle* replacementVehicle = GetVanillaVehicle(replacementVehicleID);
            Vehicle vehicle =
            {
                .Name        = vehicleNode["Name"].as<std::string>(),
                .Replacement = replacementVehicle != nullptr ? replacementVehicle : m_FallbackVehicle,
            };

            m_Vehicles[vehicleID] = vehicle;
            m_VehicleIDs.push_back(vehicleID);
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
        for (uint64_t vehicleID : m_VehicleIDs)
        {
            const Vehicle& vehicle = m_Vehicles.at(vehicleID);
            
            YAML::Node vehicleNode;
            vehicleNode["ID"]            = vehicleID;
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

const std::vector<uint64_t>& VehiclesFile::GetVehicleIDs() const
{
    return m_VehicleIDs;
}

Vehicle* VehiclesFile::GetVehicle(uint64_t vehicleID)
{
    auto it = m_Vehicles.find(vehicleID);
    return it != m_Vehicles.end() ? &(it->second) : nullptr;
}

void VehiclesFile::AddVehicle(uint64_t vehicleID, const Vehicle& vehicle)
{
    m_Vehicles[vehicleID] = vehicle;
    m_VehicleIDs.push_back(vehicleID);
}

const VanillaVehicle* VehiclesFile::GetFallbackVehicle() const
{
    return m_FallbackVehicle;
}

void VehiclesFile::SetFallbackVehicle(const VanillaVehicle* fallbackVehicle)
{
    m_FallbackVehicle = fallbackVehicle;
}