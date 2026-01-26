#pragma once


#include "core/Logger.hpp"
#include "core/Patch.hpp"


class BugFixes
{
public:
    BugFixes(const Core::Logger& logger);

public:
    void Load();
    void Unload();

private:
    const Core::Logger& m_Logger;
    
    Core::Patch m_PatchUnknownLiveryVehicleIDs;
    Core::Patch m_PatchDisabledWindow;
};
