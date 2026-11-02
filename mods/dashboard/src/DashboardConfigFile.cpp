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

        m_DashboardConfig.AlwaysVisible = yaml["AlwaysVisible"].as<bool>(m_DashboardConfig.AlwaysVisible);
        m_DashboardConfig.MetricUnits = yaml["MetricUnits"].as<bool>(m_DashboardConfig.MetricUnits);
        m_DashboardConfig.Opacity = yaml["Opacity"].as<float>(m_DashboardConfig.Opacity);
        m_DashboardConfig.Scale = yaml["Scale"].as<float>(m_DashboardConfig.Scale);
        m_DashboardConfig.FontScale = yaml["FontScale"].as<float>(m_DashboardConfig.FontScale);

        YAML::Node colorsNode = yaml["Colors"];
        m_DashboardConfig.Colors.Dial = colorsNode["Dial"].as<uint32_t>(m_DashboardConfig.Colors.Dial);
        m_DashboardConfig.Colors.Text = colorsNode["Text"].as<uint32_t>(m_DashboardConfig.Colors.Text);
        m_DashboardConfig.Colors.Needle = colorsNode["Needle"].as<uint32_t>(m_DashboardConfig.Colors.Needle);

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
