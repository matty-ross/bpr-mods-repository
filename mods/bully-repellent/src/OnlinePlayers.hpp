#pragma once


#include "BlacklistedPlayersFile.hpp"


class OnlinePlayers
{
public:
    OnlinePlayers(BlacklistedPlayersFile& blacklistedPlayersFile);

public:
    void OnGuiEventNetworkPlayerStatus(void* event);
    void OnRenderMenu();
    
private:
    BlacklistedPlayersFile& m_BlacklistedPlayersFile;
    bool m_BlacklistEnabled = true;
};
