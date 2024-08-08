#include "VehiclesFile.hpp"

#include "vendor/yaml-cpp.hpp"

#include "core/File.hpp"


static constexpr char k_Name[] = "vehicles";


VehiclesFile::VehiclesFile(const std::string& filePath, const Core::Logger& logger)
    :
    m_FilePath(filePath),
    m_Logger(logger)
{
}

void VehiclesFile::Load()
{
    try
    {
        m_Logger.Info("Loading %s from file '%s' ...", k_Name, m_FilePath.c_str());

        Core::File file(m_FilePath, Core::File::Mode::Read);
        
        YAML::Node yaml = YAML::Load(file.Read());
        {
            {
                uint64_t fallbackVehicleID = yaml["FallbackID"].as<uint64_t>();
                const VanillaVehicle* fallbackVehicle = GetVanillaVehicle(fallbackVehicleID);
                
                m_FallbackVehicle = fallbackVehicle != nullptr ? fallbackVehicle : k_LastResortFallbackVehicle;
            }

            {
                m_Vehicles.clear();
                
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
            }
        }

        m_Logger.Info("Loaded %s.", k_Name);
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load %s - %s", k_Name, e.what());
    }
}

void VehiclesFile::Save() const
{
    try
    {
        m_Logger.Info("Saving %s to file '%s' ...", k_Name, m_FilePath.c_str());

        Core::File file(m_FilePath, Core::File::Mode::Write);

        YAML::Node yaml;
        {
            {
                yaml["FallbackID"] = m_FallbackVehicle->ID;
            }

            {
                for (const Vehicle& vehicle : m_Vehicles)
                {
                    YAML::Node vehicleNode;
                    vehicleNode["ID"]            = vehicle.ID;
                    vehicleNode["Name"]          = vehicle.Name;
                    vehicleNode["ReplacementID"] = vehicle.Replacement->ID;
            
                    yaml["Vehicles"].push_back(vehicleNode);
                }
            }
        }
        file.Write(YAML::Dump(yaml));

        m_Logger.Info("Saved %s.", k_Name);
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save %s - %s", k_Name, e.what());
    }
}

std::vector<Vehicle>& VehiclesFile::GetVehicles()
{
    return m_Vehicles;
}

Vehicle* VehiclesFile::GetVehicle(uint64_t vehicleID)
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

const VanillaVehicle* VehiclesFile::GetFallbackVehicle() const
{
    return m_FallbackVehicle;
}

void VehiclesFile::SetFallbackVehicle(const VanillaVehicle* fallbackVehicle)
{
    m_FallbackVehicle = fallbackVehicle;
}
