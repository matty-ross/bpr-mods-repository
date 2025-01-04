#include "DashboardConfigFile.hpp"

#include "vendor/yaml-cpp.hpp"

#include "core/File.hpp"


static constexpr char k_Name[] = "dashboard config";


DashboardConfigFile::DashboardConfigFile(const std::string& filePath, const Core::Logger& logger)
    :
    m_FilePath(filePath),
    m_Logger(logger)
{
}

void DashboardConfigFile::Load()
{
    try
    {
        m_Logger.Info("Loading %s from file '%s' ...", k_Name, m_FilePath.c_str());

        Core::File file(m_FilePath, Core::File::Mode::Read);

        YAML::Node yaml = YAML::Load(file.ReadText());
        {
            m_DashboardConfig.DistanceDriven = yaml["DistanceDriven"].as<float>();
            m_DashboardConfig.MetricUnits    = yaml["MetricUnits"].as<bool>();
        }

        m_Logger.Info("Loaded %s.", k_Name);
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load %s - %s", k_Name, e.what());
    }
}

void DashboardConfigFile::Save() const
{
    try
    {
        m_Logger.Info("Saving %s to file '%s' ...", k_Name, m_FilePath.c_str());

        Core::File file(m_FilePath, Core::File::Mode::Write);

        YAML::Node yaml;
        {
            yaml["DistanceDriven"] = m_DashboardConfig.DistanceDriven;
            yaml["MetricUnits"]    = m_DashboardConfig.MetricUnits;
        }
        file.WriteText(YAML::Dump(yaml));

        m_Logger.Info("Saved %s.", k_Name);
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save %s - %s", k_Name, e.what());
    }
}

DashboardConfig& DashboardConfigFile::GetDashboardConfig()
{
    return m_DashboardConfig;
}
