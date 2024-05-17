#pragma once


#include <string>

#include "core/Logger.hpp"


struct Config
{
    bool UseLogger = false;
};


class ModMangerConfigFile
{
public:
    ModMangerConfigFile(const std::string& filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    Config& GetConfig();

private:
    std::string m_FilePath;

    Config m_Config;

    const Core::Logger& m_Logger;
};
