#pragma once


#include <Windows.h>

#include "core/Path.hpp"
#include "core/Logger.hpp"

#include "BugFixes.hpp"


class QualityOfLife
{
private:
    QualityOfLife();

public:
    static QualityOfLife& Get();

public:
    void OnProcessAttach();
    void OnProcessDetach();

private:
    void Load();
    void Unload();

private:
    static QualityOfLife s_Instance;

private:
    Core::Path m_ConfigDirectory;
    Core::Logger m_Logger;

    BugFixes m_BugFixes;

    HANDLE m_LoadThreadHandle = NULL;
};
