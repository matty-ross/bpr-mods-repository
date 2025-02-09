#pragma once


#include <string>

#include "core/Logger.hpp"


struct DashboardConfig
{
    bool MetricUnits = true;
    float Odometer = 0.0f;
    float Opacity = 78.0;
    struct
    {
        uint32_t Dial = 0xFFFCFF24;
        uint32_t Text = 0xFFFCFF24;
        uint32_t Needle = 0xFF3426FF;
    } Colors;
};


class DashboardConfigFile
{
public:
    DashboardConfigFile(const std::string& filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    DashboardConfig& GetDashboardConfig();

private:
    std::string m_FilePath;
    const Core::Logger& m_Logger;

    DashboardConfig m_DashboardConfig;
};
