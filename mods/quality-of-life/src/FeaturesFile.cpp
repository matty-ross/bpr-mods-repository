#include "FeaturesFile.hpp"

#include "vendor/yaml-cpp.hpp"

#include "core/File.hpp"


static constexpr char k_Name[] = "features";


FeaturesFile::FeaturesFile(Core::Path directory, const Core::Logger& logger)
    :
    m_FilePath(directory.Append("features.yaml")),
    m_Logger(logger)
{
}

void FeaturesFile::Load()
{
    try
    {
        m_Logger.Info("Loading %s from file '%s' ...", k_Name, m_FilePath.GetPath());

        Core::File file(m_FilePath.GetPath(), Core::File::Mode::Read);
        YAML::Node yaml = YAML::Load(file.Read<std::string>());
        {
            YAML::Node bugFixesNode = yaml["BugFixes"];
            m_BugFixesFeatures.UnknownLiveryVehicleIDs              = bugFixesNode["UnknownLiveryVehicleIDs"].as<bool>();
            m_BugFixesFeatures.ReloadingVehicleAfterLeavingJunkyard = bugFixesNode["ReloadingVehicleAfterLeavingJunkyard"].as<bool>();
            m_BugFixesFeatures.HudTextSpacing                       = bugFixesNode["HudTextSpacing"].as<bool>();
            m_BugFixesFeatures.IncorrectBikeRoadRules               = bugFixesNode["IncorrectBikeRoadRules"].as<bool>();
            m_BugFixesFeatures.IncorrectLobbyDeletedPopup           = bugFixesNode["IncorrectLobbyDeletedPopup"].as<bool>();
            m_BugFixesFeatures.DisabledWindow                       = bugFixesNode["DisabledWindow"].as<bool>();

            YAML::Node extraSettingsNode = yaml["ExtraSettings"];
            m_ExtraSettingsFeatures.Use24HourTimeFormat   = extraSettingsNode["Use24HourTimeFormat"].as<bool>();
            m_ExtraSettingsFeatures.UseMetricUnits        = extraSettingsNode["UseMetricUnits"].as<bool>();
            m_ExtraSettingsFeatures.DisableWebcam         = extraSettingsNode["DisableWebcam"].as<bool>();
            m_ExtraSettingsFeatures.DisableCursorClipping = extraSettingsNode["DisableCursorClipping"].as<bool>();
        }

        m_Logger.Info("Loaded %s.", k_Name);
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to load %s - %s", k_Name, e.what());
    }
}

void FeaturesFile::Save() const
{
    try
    {
        m_Logger.Info("Saving %s to file '%s' ...", k_Name, m_FilePath.GetPath());

        Core::File file(m_FilePath.GetPath(), Core::File::Mode::Write);
        YAML::Node yaml;
        {
            YAML::Node bugFixesNode;
            bugFixesNode["UnknownLiveryVehicleIDs"]              = m_BugFixesFeatures.UnknownLiveryVehicleIDs;
            bugFixesNode["ReloadingVehicleAfterLeavingJunkyard"] = m_BugFixesFeatures.ReloadingVehicleAfterLeavingJunkyard;
            bugFixesNode["HudTextSpacing"]                       = m_BugFixesFeatures.HudTextSpacing;
            bugFixesNode["IncorrectBikeRoadRules"]               = m_BugFixesFeatures.IncorrectBikeRoadRules;
            bugFixesNode["IncorrectLobbyDeletedPopup"]           = m_BugFixesFeatures.IncorrectLobbyDeletedPopup;
            bugFixesNode["DisabledWindow"]                       = m_BugFixesFeatures.DisabledWindow;
            yaml["BugFixes"] = bugFixesNode;

            YAML::Node extraSettingsNode;
            extraSettingsNode["Use24HourTimeFormat"]   = m_ExtraSettingsFeatures.Use24HourTimeFormat;
            extraSettingsNode["UseMetricUnits"]        = m_ExtraSettingsFeatures.UseMetricUnits;
            extraSettingsNode["DisableWebcam"]         = m_ExtraSettingsFeatures.DisableWebcam;
            extraSettingsNode["DisableCursorClipping"] = m_ExtraSettingsFeatures.DisableCursorClipping;
            yaml["ExtraSettings"] = extraSettingsNode;
        }
        file.Write(YAML::Dump(yaml));

        m_Logger.Info("Saved %s.", k_Name);
    }
    catch (const std::exception& e)
    {
        m_Logger.Warning("Failed to save %s - %s", k_Name, e.what());
    }
}

BugFixesFeatures& FeaturesFile::GetBugFixesFeatures()
{
    return m_BugFixesFeatures;
}

ExtraSettingsFeatures& FeaturesFile::GetExtraSettingsFeatures()
{
    return m_ExtraSettingsFeatures;
}
