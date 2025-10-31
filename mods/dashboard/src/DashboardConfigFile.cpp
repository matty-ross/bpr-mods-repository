#include "DashboardConfigFile.hpp"

#include "vendor/yaml-cpp.hpp"

#include "core/File.hpp"


static constexpr char k_Name[] = "dashboard config";


DashboardConfigFile::DashboardConfigFile(Core::Path directory, const Core::Logger& logger)
    :
    m_FilePath(directory.Append("dashboard-config.yaml")),
    m_Logger(logger)
{
}

void DashboardConfigFile::Load()
{
    try
    {
        m_Logger.Info("Loading %s from file '%s' ...", k_Name, m_FilePath.GetPath());

        Core::File file(m_FilePath.GetPath(), Core::File::Mode::Read);
        YAML::Node yaml = YAML::Load(file.Read<std::string>());
        {
            m_DashboardConfig.AlwaysVisible = yaml["AlwaysVisible"].as<bool>();
            m_DashboardConfig.MetricUnits   = yaml["MetricUnits"].as<bool>();
            m_DashboardConfig.Opacity       = yaml["Opacity"].as<float>();
            m_DashboardConfig.Scale         = yaml["Scale"].as<float>();
            
            YAML::Node colorsNode = yaml["Colors"];
            m_DashboardConfig.Colors.Dial   = colorsNode["Dial"].as<uint32_t>();
            m_DashboardConfig.Colors.Text   = colorsNode["Text"].as<uint32_t>();
            m_DashboardConfig.Colors.Needle = colorsNode["Needle"].as<uint32_t>();
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
        m_Logger.Info("Saving %s to file '%s' ...", k_Name, m_FilePath.GetPath());

        Core::File file(m_FilePath.GetPath(), Core::File::Mode::Write);
        YAML::Node yaml;
        {
            yaml["AlwaysVisible"] = m_DashboardConfig.AlwaysVisible;
            yaml["MetricUnits"]   = m_DashboardConfig.MetricUnits;
            yaml["Opacity"]       = m_DashboardConfig.Opacity;
            yaml["Scale"]         = m_DashboardConfig.Scale;

            YAML::Node colorsNode;
            colorsNode["Dial"]   = m_DashboardConfig.Colors.Dial;
            colorsNode["Text"]   = m_DashboardConfig.Colors.Text;
            colorsNode["Needle"] = m_DashboardConfig.Colors.Needle;
            yaml["Colors"] = colorsNode;
        }
        file.Write(YAML::Dump(yaml));

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
