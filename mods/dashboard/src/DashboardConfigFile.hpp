#pragma once


#include <cstdint>

#include "core/Path.hpp"
#include "core/Logger.hpp"


struct DashboardConfig
{
    bool AlwaysVisible = false;
    bool MetricUnits = true;
    float Opacity = 78.0f;
    float Scale = 1.0f;
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
    DashboardConfigFile(Core::Path directory, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    DashboardConfig& GetDashboardConfig();

private:
    Core::Path m_FilePath;
    const Core::Logger& m_Logger;

    DashboardConfig m_DashboardConfig;
};
