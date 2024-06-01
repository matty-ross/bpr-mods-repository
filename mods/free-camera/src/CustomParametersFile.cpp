#include "CustomParametersFile.hpp"

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
    try
    {
        m_Logger.Info("Loading custom parameters from file '%s' ...", m_FilePath.c_str());

        Core::File file(m_FilePath, Core::File::Mode::Read);

        YAML::Node yaml = YAML::Load(file.Read());
        {
            m_CustomParameters.clear();
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
        }

        m_Logger.Info("Loaded custom parameters.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load custom parameters - %s", e.what());
    }
}

void CustomParametersFile::Save() const
{
    try
    {
        m_Logger.Info("Saving custom parameters to file '%s' ...", m_FilePath.c_str());
        
        Core::File file(m_FilePath, Core::File::Mode::Write);

        YAML::Node yaml;
        {
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
        }
        file.Write(YAML::Dump(yaml));

        m_Logger.Info("Saved custom parameters.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save custom parameters - %s", e.what());
    }
}

std::vector<CustomParameters>& CustomParametersFile::GetCustomParameters()
{
    return m_CustomParameters;
}
