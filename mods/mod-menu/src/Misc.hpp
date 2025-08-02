#pragma once


class Misc
{
public:
    void OnPreWorldUpdate(void* gameEventQueue, void* gameActionQueue);
    void OnUpdateHudFlow();
    
    void OnRenderMenu();

private:
    bool m_PauseSimulation = false;
    bool m_UnpauseSimulation = false;

    bool m_HideHUD = false;
    bool m_ShowHUD = false;
};
