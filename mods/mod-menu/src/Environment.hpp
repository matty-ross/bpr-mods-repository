#pragma once


class Environment
{
public:
    void OnRenderMenu();

private:
    int m_TimeOfDayHours = 0;
    int m_TimeOfDayMinutes = 0;
    int m_TimeOfDayInSeconds = 0;
};
