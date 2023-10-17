#include "VehiclesFile.h"

#include <Windows.h>

#include "yaml-cpp/yaml.h"

#include "core/File.h"


VehiclesFile::VehiclesFile(const Core::Logger& logger, const std::string& filePath)
    :
    m_Logger(logger),
    m_FilePath(filePath)
{
}

void VehiclesFile::Load()
{
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