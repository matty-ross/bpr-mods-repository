#pragma once


#include "core/Pointer.hpp"

#include "BlacklistedPlayersFile.hpp"


class OnlinePlayers
{
public:
    OnlinePlayers(BlacklistedPlayersFile& blacklistedPlayersFile);

public:
    void OnGuiEventNetworkPlayerStatus(Core::Pointer guiEventNetworkPlayerStatus, Core::Pointer guiCache);
    void OnRenderMenu();
    
private:
    BlacklistedPlayersFile& m_BlacklistedPlayersFile;
    bool m_BlacklistEnabled = true;
};
