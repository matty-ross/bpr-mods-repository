#pragma once


namespace Core
{
    class Patch
    {
    public:
        Patch(void* address, size_t size);

    public:
        void Apply();
        void Remove();

    private:
        void* m_Address = nullptr;
        size_t m_Size = 0;
    };
}
