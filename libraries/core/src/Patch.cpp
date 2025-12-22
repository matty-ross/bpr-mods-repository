#include "Patch.hpp"


namespace Core
{
    Patch::Patch(void* address, size_t size)
        :
        m_Address(address),
        m_Size(size)
    {
    }
    
    void Patch::Apply()
    {
    }
    
    void Patch::Remove()
    {
    }
}
