#include "Mod.h"


namespace Core
{
    Mod::Mod(HMODULE module)
        :
        m_Module(module)
    {
    }

    HMODULE Mod::GetModule() const
    {
        return m_Module;
    }
}