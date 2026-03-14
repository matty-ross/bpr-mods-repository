#include "Enhancements.hpp"

#include "core/Pointer.hpp"


Enhancements::Enhancements(const Core::Logger& logger)
    :
    m_Logger(logger),
    m_PatchAnyVehicleTypesOnline
    {
        Core::Patch(Core::Pointer(0x00A2187F).GetAddress(), 2),
        Core::Patch(Core::Pointer(0x00A29AAD).GetAddress(), 5),
    }
{
}

void Enhancements::Load()
{
    // Apply any vehicle type online patch.
    {
        m_Logger.Info("Applying any vehicle type online patch...");

        m_PatchAnyVehicleTypesOnline[0].Apply("\xEB\x12");
        m_PatchAnyVehicleTypesOnline[1].Apply("\x90\x90\x90\x90\x90");

        m_Logger.Info("Applied any vehicle type online patch.");
    }
}

void Enhancements::Unload()
{
    // Remove any vehicle type online patch.
    {
        m_Logger.Info("Removing any vehicle type online patch...");

        m_PatchAnyVehicleTypesOnline[0].Remove();
        m_PatchAnyVehicleTypesOnline[1].Remove();

        m_Logger.Info("Removed any vehicle type online patch.");
    }
}
