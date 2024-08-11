#include "ModManagerConfigFile.hpp"

#include "vendor/yaml-cpp.hpp"

#include "core/File.hpp"


static constexpr char k_Name[] = "mod manager config";


ModManagerConfigFile::ModManagerConfigFile(const std::string& filePath, const Core::Logger& logger)
    :
    m_FilePath(filePath),
    m_Logger(logger)
{
}

void ModManagerConfigFile::Load()
{
    try
    {
        m_Logger.Info("Loading %s from file '%s' ...", k_Name, m_FilePath.c_str());

        Core::File file(m_FilePath, Core::File::Mode::Read);

        YAML::Node yaml = YAML::Load(file.Read());
        {
            {
                const YAML::Node& imguiNode = yaml["ImGui"];
                
                m_ImGuiConfig.ToggleMenusVK    = imguiNode["ToggleMenusVK"].as<int>();
                m_ImGuiConfig.ToggleOverlaysVK = imguiNode["ToggleOverlaysVK"].as<int>();
                m_ImGuiConfig.EnableDocking    = imguiNode["EnableDocking"].as<bool>();
                m_ImGuiConfig.EnableViewports  = imguiNode["EnableViewports"].as<bool>();
            }
        }

        m_Logger.Info("Loaded %s.", k_Name);
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load %s - %s", k_Name, e.what());
    }
}

void ModManagerConfigFile::Save() const
{
    try
    {
        m_Logger.Info("Saving %s to file '%s' ...", k_Name, m_FilePath.c_str());

        Core::File file(m_FilePath, Core::File::Mode::Write);

        YAML::Node yaml;
        {
            {
                YAML::Node imguiNode;
                imguiNode["ToggleMenusVK"]    = m_ImGuiConfig.ToggleMenusVK;
                imguiNode["ToggleOverlaysVK"] = m_ImGuiConfig.ToggleOverlaysVK;
                imguiNode["EnableDocking"]    = m_ImGuiConfig.EnableDocking;
                imguiNode["EnableViewports"]  = m_ImGuiConfig.EnableViewports;

                yaml["ImGui"] = imguiNode;
            }
        }
        file.Write(YAML::Dump(yaml));

        m_Logger.Info("Saved %s.", k_Name);
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save %s - %s", k_Name, e.what());
    }
}

ImGuiConfig& ModManagerConfigFile::GetImGuiConfig()
{
    return m_ImGuiConfig;
}
