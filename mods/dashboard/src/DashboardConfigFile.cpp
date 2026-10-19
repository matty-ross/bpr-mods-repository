#include <cstdint>
#include <exception>

#include <vendor/yaml-cpp.hpp>

#include <core/Path.hpp>
#include <core/Logger.hpp>
#include <core/File.hpp>

#include "DashboardConfigFile.hpp"


DashboardConfigFile::DashboardConfigFile(Core::Path configDirectoryPath, const Core::Logger& logger)
    :
    m_FilePath(configDirectoryPath.Append("dashboard-config.yaml")),
    m_Logger(logger)
{
}

DashboardConfigFile::DashboardConfig& DashboardConfigFile::GetDashboardConfig()
{
    return m_DashboardConfig;
}

void DashboardConfigFile::Load()
{
    try
    {
        YAML::Node yaml = YAML::Load(
            Core::File(m_FilePath, Core::File::Mode::Read, m_Logger).ReadAsText()
        );

        m_DashboardConfig.AlwaysVisible = yaml["AlwaysVisible"].as<bool>(false);
        m_DashboardConfig.MetricUnits = yaml["MetricUnits"].as<bool>(true);
        m_DashboardConfig.Opacity = yaml["Opacity"].as<float>(78.0f);
        m_DashboardConfig.Scale = yaml["Scale"].as<float>(1.0f);
        m_DashboardConfig.FontScale = yaml["FontScale"].as<float>(1.0f);

        YAML::Node colorsNode = yaml["Colors"];
        m_DashboardConfig.Colors.Dial = colorsNode["Dial"].as<uint32_t>(0xFFFCFF24);
        m_DashboardConfig.Colors.Text = colorsNode["Text"].as<uint32_t>(0xFFFCFF24);
        m_DashboardConfig.Colors.Needle = colorsNode["Needle"].as<uint32_t>(0xFF3426FF);

        m_Logger.Info("Loaded %s.", k_Name);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Warning("Failed to load %s. exception: %s", k_Name, ex.what());
    }
}

void DashboardConfigFile::Save() const
{
    try
    {
        YAML::Node yaml;

        yaml["AlwaysVisible"] = m_DashboardConfig.AlwaysVisible;
        yaml["MetricUnits"] = m_DashboardConfig.MetricUnits;
        yaml["Opacity"] = m_DashboardConfig.Opacity;
        yaml["Scale"] = m_DashboardConfig.Scale;
        yaml["FontScale"] = m_DashboardConfig.FontScale;

        YAML::Node colorsNode;
        colorsNode["Dial"] = m_DashboardConfig.Colors.Dial;
        colorsNode["Text"] = m_DashboardConfig.Colors.Text;
        colorsNode["Needle"] = m_DashboardConfig.Colors.Needle;
        yaml["Colors"] = colorsNode;

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
