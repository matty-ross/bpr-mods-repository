#include "HiddenStuff.hpp"

#include "core/Pointer.hpp"


HiddenStuff::HiddenStuff(const Core::Logger& logger)
    :
    m_Logger(logger),
    m_PatchPlaneVehicleType(Core::Pointer(0x06A701B0).GetAddress(), 18)
{
}

void HiddenStuff::Load()
{
    // Apply plane vehicle type patch.
    {
        m_Logger.Info("Applying plane vehicle type patch...");

        m_PatchPlaneVehicleType.Apply("\x83\xF9\x02\x0F\x96\xC1\x0F\xB6\xC9\x90\x90\x90\x90\x90\x90\x90\x90\x90");

        m_Logger.Info("Applied plane vehicle type patch.");
    }
}

void HiddenStuff::Unload()
{
    // Remove plane vehicle type patch.
    {
        m_Logger.Info("Removing plane vehicle type patch...");

        m_PatchPlaneVehicleType.Remove();

        m_Logger.Info("Removed plane vehicle type patch.");
    }
}
