#pragma once


#include <cstdint>


namespace Core
{
    class Pointer
    {
    public:
        __forceinline Pointer(void* address)
            :
            m_Address(address)
        {
        }

        __forceinline Pointer(uintptr_t address)
            :
            m_Address(reinterpret_cast<void*>(address))
        {
        }

    public:
        template <typename T = void*>
        __forceinline T GetAddress() const
        {
            return static_cast<T>(m_Address);
        }

        __forceinline Pointer deref() const
        {
            return *(static_cast<void**>(m_Address));
        }

        __forceinline Pointer at(ptrdiff_t offset) const
        {
            return reinterpret_cast<uintptr_t>(m_Address) + offset;
        }

        template <typename T>
        __forceinline T& as() const
        {
            return *(static_cast<T*>(m_Address));
        }

    private:
        void* m_Address = nullptr;
    };
}
