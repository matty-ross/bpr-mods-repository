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
    
    void Mod::OnProcessAttach()
    {
        PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpThreadParameter) -> DWORD
        {
            static_cast<Mod*>(lpThreadParameter)->Load();
            return 0;
        };
        m_LoadThread = CreateThread(nullptr, 0, loadThreadProc, this, 0, nullptr);
    }
    
    void Mod::OnProcessDetach()
    {
        CloseHandle(m_LoadThread);
        Unload();
    }
    
    void Mod::OnThreadAttach()
    {
    }
    
    void Mod::OnThreadDetach()
    {
    }
}