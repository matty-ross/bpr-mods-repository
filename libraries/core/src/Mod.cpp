#include "Mod.h"


namespace Core
{
    Mod::Mod(HMODULE module)
        :
        m_Module(module)
    {
        m_LoadThread = CreateThread(nullptr, 0, &Mod::LoadThreadProc, this, CREATE_SUSPENDED, nullptr);
    }

    Mod::~Mod()
    {
        CloseHandle(m_LoadThread);
    }

    HMODULE Mod::GetModule() const
    {
        return m_Module;
    }

    void Mod::StartLoading() const
    {
        ResumeThread(m_LoadThread);
    }

    void Mod::WaitTillLoaded(DWORD timeout) const
    {
        WaitForSingleObject(m_LoadThread, timeout);
    }

    DWORD CALLBACK Mod::LoadThreadProc(LPVOID lpParameter)
    {
        static_cast<Mod*>(lpParameter)->Load();
        return 0;
    }
}