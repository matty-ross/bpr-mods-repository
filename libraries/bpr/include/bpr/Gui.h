#pragma once


namespace BPR
{
    inline const char* GetLocalizedString(const char* stringID)
    {
        const char* localizedString = nullptr;

        __asm
        {
            push dword ptr [stringID]
            mov ecx, dword ptr ds:[0x013FC8E0]
            add ecx, 0x7A0E34

            mov eax, 0x0089C6D0
            call eax

            mov dword ptr [localizedString], eax
        }

        return localizedString;
    }
}