#include "HiddenStuff.hpp"

#include <cstdio>

#include "core/Pointer.hpp"


HiddenStuff::HiddenStuff(const Core::Logger& logger, HiddenStuffFeatures& hiddenStuffFeatures)
    :
    m_Logger(logger),
    m_HiddenStuffFeatures(hiddenStuffFeatures),
    m_PatchDevLog(Core::Pointer(0x00F0280C).GetAddress(), 4),
    m_PatchPlaneVehicleType(Core::Pointer(0x06A701B0).GetAddress(), 18)
{
}

void HiddenStuff::Load()
{
    if (m_HiddenStuffFeatures.DevLog)
    {
        // Apply dev log patch.
        {
            m_Logger.Info("Applying dev log patch...");

            FILE* newStdout;
            freopen_s(&newStdout, "CONOUT$", "w", stdout);

            void* devLog = &HiddenStuff::DevLog;
            m_PatchDevLog.Apply(&devLog);

            m_Logger.Info("Applied dev log patch.");
        }
    }

    if (m_HiddenStuffFeatures.SatNavRotation)
    {
        // Enable Sat-Nav rotation.
        {
            Core::Pointer(0x0135AF0D).as<bool>() = true;
        }
    }

    if (m_HiddenStuffFeatures.PlaneVehicleType)
    {
        // Apply plane vehicle type patch.
        {
            m_Logger.Info("Applying plane vehicle type patch...");

            m_PatchPlaneVehicleType.Apply("\x83\xF9\x02\x0F\x96\xC1\x0F\xB6\xC9\x90\x90\x90\x90\x90\x90\x90\x90\x90");

            m_Logger.Info("Applied plane vehicle type patch.");
        }
    }
}

void HiddenStuff::Unload()
{
    if (m_HiddenStuffFeatures.PlaneVehicleType)
    {
        // Remove plane vehicle type patch.
        {
            m_Logger.Info("Removing plane vehicle type patch...");

            m_PatchPlaneVehicleType.Remove();

            m_Logger.Info("Removed plane vehicle type patch.");
        }
    }

    if (m_HiddenStuffFeatures.SatNavRotation)
    {
        // Disable Sat-Nav rotation.
        {
            Core::Pointer(0x0135AF0D).as<bool>() = false;
        }
    }

    if (m_HiddenStuffFeatures.DevLog)
    {
        // Remove dev log patch.
        {
            m_Logger.Info("Removing dev log patch...");

            m_PatchDevLog.Remove();

            m_Logger.Info("Removed dev log patch.");
        }
    }
}

__declspec(naked) void HiddenStuff::DevLog()
{
    static constexpr char formatString[] = "%s";

    __asm
    {
        push ebp
        mov ebp, esp
        pushfd
        pushad
        mov edi, ecx

        lea eax, [edi+8]
        push eax
        call dword ptr ds:[0x0828227C] // EnterCriticalSection

        push dword ptr [ebp+8]
        push offset formatString
        call printf_s
        add esp, 8

        lea eax, [edi+8]
        push eax
        call dword ptr ds:[0x08282350] // LeaveCriticalSection

        popad
        popfd
        pop ebp
        ret 4
    }
}
