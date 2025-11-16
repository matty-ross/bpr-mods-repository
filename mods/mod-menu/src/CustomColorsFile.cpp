#include "CustomColorsFile.hpp"

#include "vendor/yaml-cpp.hpp"

#include "core/File.hpp"


static constexpr char k_Name[] = "custom colors";


CustomColorsFile::CustomColorsFile(Core::Path directory, const Core::Logger& logger)
    :
    m_FilePath(directory.Append("custom-colors.yaml")),
    m_Logger(logger)
{
}

void CustomColorsFile::Load()
{
    try
    {
        m_Logger.Info("Loading %s from file '%s' ...", k_Name, m_FilePath.GetPath());

        Core::File file(m_FilePath.GetPath(), Core::File::Mode::Read);
        YAML::Node yaml = YAML::Load(file.Read<std::string>());
        {
            m_CustomColors.clear();
            for (YAML::Node customColorNode : yaml["CustomColors"])
            {
                CustomColor customColor =
                {
                    .Name                = customColorNode["Name"].as<std::string>(),
                    .PaintColor          = customColorNode["PaintColor"].as<std::array<float, 3>>(),
                    .PaintColorIntensity = customColorNode["PaintColorIntensity"].as<std::array<float, 3>>(),
                    .PearlColor          = customColorNode["PearlColor"].as<std::array<float, 3>>(),
                    .PearlColorIntensity = customColorNode["PearlColorIntensity"].as<std::array<float, 3>>(),
                };
                m_CustomColors.push_back(customColor);
            }
        }

        m_Logger.Info("Loaded %s.", k_Name);
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load %s - %s", k_Name, e.what());
    }
}

void CustomColorsFile::Save() const
{
    try
    {
        m_Logger.Info("Saving %s to file '%s' ...", k_Name, m_FilePath.GetPath());

        Core::File file(m_FilePath.GetPath(), Core::File::Mode::Write);
        YAML::Node yaml;
        {
            for (const CustomColor& customColor : m_CustomColors)
            {
                YAML::Node customColorNode;
                customColorNode["Name"]                = customColor.Name;
                customColorNode["PaintColor"]          = customColor.PaintColor;
                customColorNode["PaintColorIntensity"] = customColor.PaintColorIntensity;
                customColorNode["PearlColor"]          = customColor.PearlColor;
                customColorNode["PearlColorIntensity"] = customColor.PearlColorIntensity;
                yaml["CustomColors"].push_back(customColorNode);
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

std::vector<CustomColor>& CustomColorsFile::GetCustomColors()
{
    return m_CustomColors;
}
