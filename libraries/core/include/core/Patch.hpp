#pragma once


#include <cstdint>


namespace Core
{
    class Patch
    {
    public:
        Patch(void* address, size_t size);
        ~Patch();

    public:
        void Apply(const void* bytes);
        void Remove();

    private:
        void* m_Address = nullptr;
        size_t m_Size = 0;

        uint8_t* m_OriginalBytes = nullptr;
    };
}
