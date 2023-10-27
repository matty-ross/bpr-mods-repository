#include "VehiclesFile.h"

#include <Windows.h>

#include "yaml-cpp/yaml.h"

#include "core/File.h"

#include "bpr/CgsID.h"


VehiclesFile::VehiclesFile(const Core::Logger& logger, const std::string& filePath)
    :
    m_Logger(logger),
    m_FilePath(filePath)
{
}

void VehiclesFile::Load()
{
    try
    {
        m_Logger.Info("Loading vehicles from file '%s' ...", m_FilePath.c_str());

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

        YAML::Node yaml = YAML::Load(readFile());

        m_Vehicles.clear();
        m_VehicleIDs.clear();

        for (const YAML::Node& vehicleNode : yaml)
        {
            std::string newID = vehicleNode["NewID"].as<std::string>();
            std::string replacementID = vehicleNode["ReplacementID"].as<std::string>();
            
            const VehicleID* replacementVehicleID = GetVanillaVehicleID(BPR::CgsID_Compress(replacementID.c_str()));

            Vehicle vehicle = {};
            vehicle.NewID.Compressed = BPR::CgsID_Compress(newID.c_str());
            strcpy_s(vehicle.NewID.Uncompressed, newID.c_str());
            vehicle.ReplacementID = replacementVehicleID != nullptr ? replacementVehicleID : &k_FallbackVehicleID;
            
            m_Vehicles[vehicle.NewID.Compressed] = vehicle;
            m_VehicleIDs.push_back(vehicle.NewID.Compressed);
        }

        m_Logger.Info("Loaded vehicles.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load vehicles. %s", e.what());
    }
}

void VehiclesFile::Save()
{
    try
    {
        m_Logger.Info("Saving vehicles to file '%s' ...", m_FilePath.c_str());

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

        YAML::Node yaml;

        for (uint64_t vehicleID : m_VehicleIDs)
        {
            const Vehicle& vehicle = m_Vehicles.at(vehicleID);
            YAML::Node vehicleNode;
            {
                vehicleNode["NewID"]         = vehicle.NewID.Uncompressed;
                vehicleNode["ReplacementID"] = vehicle.ReplacementID->Uncompressed;
            }
            
            yaml.push_back(vehicleNode);
        }

        writeFile(YAML::Dump(yaml));

        m_Logger.Info("Saved vehicles.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save vehicles. %s", e.what());
    }
}

std::map<uint64_t, Vehicle>& VehiclesFile::GetVehicles()
{
    return m_Vehicles;
}

const std::vector<uint64_t>& VehiclesFile::GetVehicleIDs() const
{
    return m_VehicleIDs;
}

void VehiclesFile::AddVehicle(const Vehicle& vehicle)
{
    uint64_t vehicleID = vehicle.NewID.Compressed;
    m_Vehicles[vehicleID] = vehicle;
    m_VehicleIDs.push_back(vehicleID);
}