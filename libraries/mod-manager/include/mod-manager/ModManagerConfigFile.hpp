#pragma once


#include <string>

#include "core/Logger.hpp"


struct ImGuiConfig
{
    bool EnableDocking = true;
    bool EnableViewports = false;
};


class ModManagerConfigFile
{
public:
    ModManagerConfigFile(const std::string& filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    ImGuiConfig& GetImGuiConfig();

private:
    std::string m_FilePath;
    const Core::Logger& m_Logger;

    ImGuiConfig m_ImGuiConfig;
};
