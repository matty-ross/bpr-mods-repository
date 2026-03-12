#pragma once


#include "core/Logger.hpp"
#include "core/Patch.hpp"

#include "FeaturesFile.hpp"


class HiddenStuff
{
public:
    HiddenStuff(const Core::Logger& logger, HiddenStuffFeatures& hiddenStuffFeatures);

public:
    void Load();
    void Unload();

private:
    static void DevLog();

private:
    const Core::Logger& m_Logger;
    HiddenStuffFeatures& m_HiddenStuffFeatures;

    Core::Patch m_PatchDevLog;
    Core::Patch m_PatchPlaneVehicleType;
};
