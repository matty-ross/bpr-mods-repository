#pragma once


#include "core/Path.hpp"
#include "core/Logger.hpp"

#include "BlacklistedPlayersFile.hpp"
#include "OnlinePlayers.hpp"


class BullyRepellent
{
private:
    BullyRepellent();

public:
    static BullyRepellent& Get();

public:
    void Load();
    void Unload();

private:
    void RenderMenu();

private:
    static constexpr char k_Name[] = "Bully Repellent";
    static constexpr char k_Version[] = "2.0.0";
    static constexpr char k_Author[] = "PISros0724 (Matty)";
    static constexpr char k_ConfigDirectoryPath[] = "bully-repellent\\";

    static BullyRepellent s_Instance;

private:
    Core::Logger m_Logger;

    Core::Path m_ConfigDirectoryPath;

    BlacklistedPlayersFile m_BlacklistedPlayersFile;

    OnlinePlayers m_OnlinePlayers;
};
