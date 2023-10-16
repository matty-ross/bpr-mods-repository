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

        // TODO: use the uncompressed vehicle IDss

        YAML::Node yaml;
        {
            yaml["FallbackVehicleID"] = m_FallbackVehicleID;
        }

        for (uint64_t vehicleID : m_VehicleIDs)
        {
            uint64_t replacementVehicleID = m_Vehicles.at(vehicleID);
            
            char key[32] = {};
            sprintf_s(key, "%llu", replacementVehicleID);
            yaml["Vehicles"][key] = replacementVehicleID;
        }

        writeFile(YAML::Dump(yaml));

        m_Logger.Info("Saved vehicles.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save vehicles. %s", e.what());
    }
}

uint64_t VehiclesFile::GetFallbackVehicleID() const
{
    return m_FallbackVehicleID;
}

void VehiclesFile::SetFallbackVehicleID(uint64_t fallbackVehicleID)
{
    m_FallbackVehicleID = fallbackVehicleID;
}

std::map<uint64_t, uint64_t>& VehiclesFile::GetVehicles()
{
    return m_Vehicles;
}

const std::vector<uint64_t>& VehiclesFile::GetVehicleIDs() const
{
    return m_VehicleIDs;
}

void VehiclesFile::AddVehicle(uint64_t vehicleID, uint64_t vehicleReplacementID)
{
    m_Vehicles[vehicleID] = vehicleReplacementID;
    m_VehicleIDs.push_back(vehicleID);
}