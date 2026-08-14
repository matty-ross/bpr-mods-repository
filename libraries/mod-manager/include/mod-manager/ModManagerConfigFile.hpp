#pragma once


#include <Windows.h>

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

        int ToggleMenusVK = VK_F7;
        int ToggleOverlaysVK = VK_F8;
        StyleColors StyleColors = StyleColors::Classic;
        float FontScale = 1.0f;
        bool EnableDocking = true;
        bool EnableViewports = false;
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

    ImGuiConfig m_ImGuiConfig;

    const Core::Logger& m_Logger;
};
