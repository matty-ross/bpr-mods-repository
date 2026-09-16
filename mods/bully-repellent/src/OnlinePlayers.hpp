#pragma once


#include "core/Pointer.hpp"

#include "BlacklistedPlayersFile.hpp"


class OnlinePlayers
{
public:
    OnlinePlayers(BlacklistedPlayersFile& blacklistedPlayersFile);

public:
    void CheckPlayers(Core::Pointer guiOutEventQueue);
    void RenderMenu();

private:
    BlacklistedPlayersFile& m_BlacklistedPlayersFile;
    bool m_BlacklistEnabled = true;
};
