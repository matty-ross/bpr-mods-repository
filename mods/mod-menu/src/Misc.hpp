#pragma once


class Misc
{
public:
    void OnPreWorldUpdate(void* gameEventQueue, void* gameActionQueue);
    
    void OnRenderMenu();

private:
    bool m_PauseSimulation = false;
    bool m_UnpauseSimulation = false;
};
