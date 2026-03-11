#pragma once


#include "core/Logger.hpp"
#include "core/Patch.hpp"


class HiddenStuff
{
public:
    HiddenStuff(const Core::Logger& logger);

public:
    void Load();
    void Unload();

private:
    static void DevLog();

private:
    const Core::Logger& m_Logger;

    Core::Patch m_PatchDevLog;
    Core::Patch m_PatchPlaneVehicleType;
};
