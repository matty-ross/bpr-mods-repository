#pragma once


#include "core/Path.hpp"
#include "core/Logger.hpp"

#include "VehiclesFile.hpp"
#include "ChallengesFile.hpp"
#include "VehicleProtection.hpp"
#include "ChallengeProtection.hpp"


class Protection
{
private:
    Protection();

public:
    static Protection& Get();

public:
    void Load();
    void Unload();

private:
    void DeferredLoad();

    void RenderMenu();

private:
    static constexpr char k_Name[] = "Protection";
    static constexpr char k_Version[] = "2.0.0";
    static constexpr char k_Author[] = "PISros0724 (Matty)";
    static constexpr char k_ConfigDirectoryPath[] = "protection\\";

    static Protection s_Instance;

private:
    Core::Logger m_Logger;
    
    Core::Path m_ConfigDirectoryPath;

    VehiclesFile m_VehiclesFile;
    ChallengesFile m_ChallengesFile;

    VehicleProtection m_VehicleProtection;
    ChallengeProtection m_ChallengeProtection;
};
