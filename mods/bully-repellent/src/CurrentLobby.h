#pragma once


#include "core/Pointer.h"

#include "BlacklistedPlayersFile.h"


class CurrentLobby
{
public:
    CurrentLobby(BlacklistedPlayersFile& blacklistedPlayersFile);

public:
    void OnUpdate(Core::Pointer guiEventNetworkPlayerStatus);
    void OnRenderMenu();
    
private:
    BlacklistedPlayersFile& m_BlacklistedPlayersFile;
};