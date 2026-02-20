#pragma once


#include <Windows.h>

#include "core/Path.hpp"
#include "core/Logger.hpp"

#include "FeaturesFile.hpp"
#include "BugFixes.hpp"
#include "ExtraSettings.hpp"


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

    FeaturesFile m_FeaturesFile;

    BugFixes m_BugFixes;
    ExtraSettings m_ExtraSettings;

    HANDLE m_LoadThreadHandle = NULL;
};
