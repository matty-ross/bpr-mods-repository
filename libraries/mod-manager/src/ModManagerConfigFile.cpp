#include <exception>
#include <Windows.h>

#include "vendor/yaml-cpp.hpp"

#include "core/Path.hpp"
#include "core/Logger.hpp"
#include "core/File.hpp"
#include "mod-manager/ModManagerConfigFile.hpp"


ModManagerConfigFile::ModManagerConfigFile(Core::Path configDirectory, const Core::Logger& logger)
    :
    m_FilePath(configDirectory.Append("mod-manager-config.yaml")),
    m_Logger(logger)
{
}

ModManagerConfigFile::ImGuiConfig& ModManagerConfigFile::GetImGuiConfig()
{
    return m_ImGuiConfig;
}

void ModManagerConfigFile::Load()
{
    try
    {
        YAML::Node yaml = YAML::Load(
            Core::File(m_FilePath, Core::File::Mode::Read, m_Logger).ReadAsText()
        );

        YAML::Node imguiNode = yaml["ImGui"];
        m_ImGuiConfig.ToggleMenusVK = imguiNode["ToggleMenusVK"].as<int>(VK_F7);
        m_ImGuiConfig.ToggleOverlaysVK = imguiNode["ToggleOverlaysVK"].as<int>(VK_F8);
        m_ImGuiConfig.StyleColors = static_cast<enum class ImGuiConfig::StyleColors>(imguiNode["StyleColors"].as<int>(static_cast<int>(ImGuiConfig::StyleColors::Classic)));
        m_ImGuiConfig.EnableDocking = imguiNode["EnableDocking"].as<bool>(true);
        m_ImGuiConfig.EnableViewports = imguiNode["EnableViewports"].as<bool>(false);

        m_Logger.Info("Loaded %s.", k_Name);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Warning("Failed to load %s. exception: %s", k_Name, ex.what());
    }
}

void ModManagerConfigFile::Save() const
{
    try
    {
        YAML::Node yaml;

        YAML::Node imguiNode;
        imguiNode["ToggleMenusVK"] = m_ImGuiConfig.ToggleMenusVK;
        imguiNode["ToggleOverlaysVK"] = m_ImGuiConfig.ToggleOverlaysVK;
        imguiNode["StyleColors"] = static_cast<int>(m_ImGuiConfig.StyleColors);
        imguiNode["EnableDocking"] = m_ImGuiConfig.EnableDocking;
        imguiNode["EnableViewports"] = m_ImGuiConfig.EnableViewports;
        yaml["ImGui"] = imguiNode;

        Core::File(m_FilePath, Core::File::Mode::Write, m_Logger).WriteAsText(
            YAML::Dump(yaml)
        );

        m_Logger.Info("Saved %s.", k_Name);
    }
    catch (const std::exception& ex)
    {
        m_Logger.Warning("Failed to save %s. exception: %s", k_Name, ex.what());
    }
}
