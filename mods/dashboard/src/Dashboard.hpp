#pragma once


#include <core/Path.hpp>
#include <core/Logger.hpp>

#include "DashboardConfigFile.hpp"
#include "DashboardHud.hpp"


class Dashboard
{
private:
    Dashboard();

public:
    static Dashboard& Get();

public:
    void Load();
    void Unload();

private:
    void DeferredLoad();

    void RenderMenu();

private:
    static constexpr char k_Name[] = "Dashboard";
    static constexpr char k_Version[] = "2.0.0";
    static constexpr char k_Author[] = "PISros0724 (Matty)";
    static constexpr char k_ConfigDirectoryPath[] = "dashboard\\";

    static Dashboard s_Instance;

private:
    Core::Logger m_Logger;
    
    Core::Path m_ConfigDirectoryPath;
    Core::Path m_AssetsDirectoryPath;

    DashboardConfigFile m_DashboardConfigFile;

    DashboardHud m_DashboardHud;
};
