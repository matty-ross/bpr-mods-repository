#pragma once


#include <string>

#include "core/Logger.hpp"


class DashboardConfigFile
{
public:
    DashboardConfigFile(const std::string& filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

private:
    std::string m_FilePath;
    const Core::Logger& m_Logger;
};
