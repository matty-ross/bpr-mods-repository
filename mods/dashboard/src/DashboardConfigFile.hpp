#pragma once


#include <string>

#include "core/Logger.hpp"


struct DashboardConfig
{
    float Odometer = 0.0f;
    bool MetricUnits = true;
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
