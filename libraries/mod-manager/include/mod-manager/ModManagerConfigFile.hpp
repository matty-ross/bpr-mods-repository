#pragma once


#include "core/Path.hpp"
#include "core/Logger.hpp"


class ModManagerConfigFile
{
public:
    struct ImGuiConfig
    {
        enum class StyleColors
        {
            Classic,
            Dark,
            Light,
        };

        int ToggleMenusVK;
        int ToggleOverlaysVK;
        StyleColors StyleColors;
        bool EnableDocking;
        bool EnableViewports;
    };

public:
    ModManagerConfigFile(Core::Path configDirectory, const Core::Logger& logger);

public:
    ImGuiConfig& GetImGuiConfig();

    void Load();
    void Save() const;

private:
    static constexpr char k_Name[] = "mod manager config";

private:
    Core::Path m_FilePath;

    ImGuiConfig m_ImGuiConfig = {};

    const Core::Logger& m_Logger;
};
