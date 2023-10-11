#pragma once


#include "core/Pointer.h"

#include "BlacklistedPlayersFile.h"


class OnlinePlayers
{
public:
    OnlinePlayers(BlacklistedPlayersFile& blacklistedPlayersFile);

public:
    void OnUpdate(Core::Pointer guiEventNetworkPlayerStatus);
    void OnRenderMenu();
    
private:
    BlacklistedPlayersFile& m_BlacklistedPlayersFile;

    bool m_BlacklistEnabled = true;
};