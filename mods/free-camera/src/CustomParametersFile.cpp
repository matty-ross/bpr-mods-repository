#include "CustomParametersFile.hpp"

#include <stdexcept>

#include "vendor/yaml-cpp.hpp"

#include "core/File.hpp"


CustomParametersFile::CustomParametersFile(const std::string& filePath, const Core::Logger& logger)
    :
    m_FilePath(filePath),
    m_Logger(logger)
{
}

void CustomParametersFile::Load()
{
    auto readFile = [this]() -> std::string
    {
        try
        {
            Core::File file(m_FilePath, Core::File::Operation::Read);
            return file.Read();
        }
        catch (const std::runtime_error& e)
        {
            m_Logger.Warning("%s. Last error: 0x%08X.", e.what(), GetLastError());
        }

        return std::string();
    };

    m_Valid = false;

    try
    {
        m_Logger.Info("Loading custom parameters from file '%s' ...", m_FilePath.c_str());

        m_CustomParameters.clear();

        YAML::Node yaml = YAML::Load(readFile());
        for (const YAML::Node& customParametersNode : yaml["CustomParameters"])
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
                .DropFactor               = customParametersNode["DropFactor"].as<float>(),
            };

            m_CustomParameters.push_back(customParameters);
        }

        m_Logger.Info("Loaded custom parameters.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load custom parameters. %s", e.what());
    }

    m_Valid = true;
}

void CustomParametersFile::Save() const
{
    auto writeFile = [this](const std::string& content) -> void
    {
        try
        {
            Core::File file(m_FilePath, Core::File::Operation::Write);
            file.Write(content);
        }
        catch (const std::runtime_error& e)
        {
            m_Logger.Warning("%s. Last error: 0x%08X.", e.what(), GetLastError());
        }
    };

    if (!m_Valid)
    {
        return;
    }

    try
    {
        m_Logger.Info("Saving custom parameters to file '%s' ...", m_FilePath.c_str());

        YAML::Node yaml;
        for (const CustomParameters& customParameters : m_CustomParameters)
        {
            YAML::Node customParametersNode;
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

            yaml["CustomParameters"].push_back(customParametersNode);
        }
        writeFile(YAML::Dump(yaml));

        m_Logger.Info("Saved custom parameters.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save custom parameters. %s", e.what());
    }
}

std::vector<CustomParameters>& CustomParametersFile::GetCustomParameters()
{
    return m_CustomParameters;
}
