#pragma once


class Environment
{
public:
    void OnRenderMenu();

private:
    bool m_OverrideTrafficDensity = false;
    bool m_OverrideWeather = false;
};
