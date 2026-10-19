#pragma once


#include <cstdint>

#include <core/Path.hpp>
#include <core/Logger.hpp>


class DashboardConfigFile
{
public:
    struct DashboardConfig
    {
        bool AlwaysVisible = false;
        bool MetricUnits = true;
        float Opacity = 78.0f;
        float Scale = 1.0f;
        float FontScale = 1.0f;
        struct
        {
            uint32_t Dial = 0xFFFCFF24;
            uint32_t Text = 0xFFFCFF24;
            uint32_t Needle = 0xFF3426FF;
        } Colors;
    };

public:
    DashboardConfigFile(Core::Path configDirectoryPath, const Core::Logger& logger);

public:
    DashboardConfig& GetDashboardConfig();

    void Load();
    void Save() const;

private:
    static constexpr char k_Name[] = "dashboard config";

private:
    Core::Path m_FilePath;

    DashboardConfig m_DashboardConfig;

    const Core::Logger& m_Logger;
};
