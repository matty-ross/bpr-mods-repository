#pragma once


#include <cstdint>

#include "core/Logger.hpp"


struct DashboardConfig
{
    bool MetricUnits = true;
    float Opacity = 78.0f;
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
    DashboardConfigFile(const char* filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    DashboardConfig& GetDashboardConfig();

private:
    const char* m_FilePath;
    const Core::Logger& m_Logger;

    DashboardConfig m_DashboardConfig;
};
