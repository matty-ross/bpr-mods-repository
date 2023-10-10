#include "CustomParametersFile.h"

#include <Windows.h>

#include "imgui/imgui.h"

#include "yaml-cpp/yaml.h"

#include "core/File.h"


CustomParametersFile::CustomParametersFile(const Core::Logger& logger, const std::string& filePath)
    :
    m_Logger(logger),
    m_FilePath(filePath)
{
}

void CustomParametersFile::Load()
{
    try
    {
        m_Logger.Info("Loading custom parameters from file '%s' ...", m_FilePath.c_str());

        auto readFile = [this]() -> std::string
        {
            try
            {
                Core::File file(m_FilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_ALWAYS);
                return file.Read();
            }
            catch (const std::runtime_error& e)
            {
                m_Logger.Warning("%s. Last error: 0x%08X.", e.what(), GetLastError());
            }

            return std::string();
        };

        YAML::Node yaml = YAML::Load(readFile());

        m_CustomParameters.clear();

        for (const YAML::Node& customParametersNode : yaml)
        {
            CustomParameters customParameters =
            {
                .Name                     = customParametersNode["Name"].as<std::string>(),
                .PitchSpring              = customParametersNode["PitchSpring"].as<float>(),
                .YawSpring                = customParametersNode["YawSpring"].as<float>(),
                .PivotY                   = customParametersNode["PivotY"].as<float>(),
                .PivotZ                   = customParametersNode["PivotZ"].as<float>(),
                .PivotZOffset             = customParametersNode["PivotZOffset"].as<float>(),
                .FOV                      = customParametersNode["FOV"].as<float>(),
                .InFrontFOVMax            = customParametersNode["InFrontFOVMax"].as<float>(),
                .FrontInAmount            = customParametersNode["FrontInAmount"].as<float>(),
                .DriftYawSpring           = customParametersNode["DriftYawSpring"].as<float>(),
                .BoostFOVZoomCompensation = customParametersNode["BoostFOVZoomCompensation"].as<float>(),
                .DownAngle                = customParametersNode["DownAngle"].as<float>(),
                .DropFactor               = customParametersNode["DropFactor"].as<float>()
            };

            m_CustomParameters.push_back(customParameters);
        }

        m_Logger.Info("Loaded custom parameters.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load custom parameters. %s", e.what());
    }
}

void CustomParametersFile::Save()
{
    try
    {
        m_Logger.Info("Saving custom parameters to file '%s' ...", m_FilePath.c_str());

        auto writeFile = [this](const std::string& content) -> void
        {
            try
            {
                Core::File file(m_FilePath, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS);
                file.Write(content);
            }
            catch (const std::runtime_error& e)
            {
                m_Logger.Warning("%s. Last error: 0x%08X.", e.what(), GetLastError());
            }
        };

        YAML::Node yaml;

        for (const CustomParameters& customParameters : m_CustomParameters)
        {
            YAML::Node customParametersNode;
            {
                customParametersNode["Name"]                     = customParameters.Name;
                customParametersNode["PitchSpring"]              = customParameters.PitchSpring;
                customParametersNode["YawSpring"]                = customParameters.YawSpring;
                customParametersNode["PivotY"]                   = customParameters.PivotY;
                customParametersNode["PivotZ"]                   = customParameters.PivotZ;
                customParametersNode["PivotZOffset"]             = customParameters.PivotZOffset;
                customParametersNode["FOV"]                      = customParameters.FOV;
                customParametersNode["InFrontFOVMax"]            = customParameters.InFrontFOVMax;
                customParametersNode["FrontInAmount"]            = customParameters.FrontInAmount;
                customParametersNode["DriftYawSpring"]           = customParameters.DriftYawSpring;
                customParametersNode["BoostFOVZoomCompensation"] = customParameters.BoostFOVZoomCompensation;
                customParametersNode["DownAngle"]                = customParameters.DownAngle;
                customParametersNode["DropFactor"]               = customParameters.DropFactor;
            }

            yaml.push_back(customParametersNode);
        }

        writeFile(YAML::Dump(yaml));

        m_Logger.Info("Saved custom parameters.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save custom parameters. %s", e.what());
    }
}

const std::vector<CustomParameters>& CustomParametersFile::GetCustomParameters() const
{
    return m_CustomParameters;
}

void CustomParametersFile::AddCustomParameters(const CustomParameters& customParameters)
{
    m_CustomParameters.push_back(customParameters);
}