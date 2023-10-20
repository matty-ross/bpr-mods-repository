#pragma once


#include <cstdint>


namespace BPR
{
    uint64_t CgsID_Compress(const char* string)
    {
        uint64_t id = 0x0000000000000000;

        __asm
        {
            mov ecx, dword ptr [string]

            mov eax, 0x0086CED0
            call eax

            mov dword ptr [id + 0x0], eax
            mov dword ptr [id + 0x4], edx
        }

        return id;
    }

    void CgsID_Uncompress(uint64_t id, char string[13])
    {
        __asm
        {
            mov ecx, dword ptr [string]
            push dword ptr [id + 0x4]
            push dword ptr [id + 0x0]

            mov eax, 0x0086CE40
            call eax
            add esp, 0x8
        }
    }

    void CgsID_ConvertToString(uint64_t id, char string[13])
    {
        __asm
        {
            mov ecx, dword ptr [string]
            push dword ptr [id + 0x4]
            push dword ptr [id + 0x0]

            mov eax, 0x0086CE00
            call eax
            add esp, 0x8
        }
    }
}