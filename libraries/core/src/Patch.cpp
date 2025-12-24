#include "Patch.hpp"

#include <cstring>
#include <Windows.h>


namespace Core
{
    Patch::Patch(void* address, size_t size)
        :
        m_Address(address),
        m_Size(size)
    {
        m_OriginalBytes = new uint8_t[size];
    }

    Patch::~Patch()
    {
        delete[] m_OriginalBytes;
    }
    
    void Patch::Apply(const void* bytes)
    {
        DWORD oldProtection = 0;
        VirtualProtect(m_Address, m_Size, PAGE_EXECUTE_READWRITE, &oldProtection);

        memcpy_s(m_OriginalBytes, m_Size, m_Address, m_Size);
        memcpy_s(m_Address, m_Size, bytes, m_Size);

        VirtualProtect(m_Address, m_Size, oldProtection, &oldProtection);
    }
    
    void Patch::Remove()
    {
        DWORD oldProtection = 0;
        VirtualProtect(m_Address, m_Size, PAGE_EXECUTE_READWRITE, &oldProtection);

        memcpy_s(m_Address, m_Size, m_OriginalBytes, m_Size);

        VirtualProtect(m_Address, m_Size, oldProtection, &oldProtection);
    }
}
