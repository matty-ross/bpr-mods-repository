#pragma once


#include <string>
#include <vector>
#include <map>

#include "core/Logger.h"
#include "core/Pointer.h"


class CurrentLobby
{
public:
    CurrentLobby(Core::Logger& logger, const std::string& blacklistedPlayersFilePath);

public:
    void OnUpdate(Core::Pointer guiEventNetworkPlayerStatus);
    void OnRenderMenu();
    
    void LoadBlacklistedPlayers();
    void SaveBlacklistedPlayers();

private:
    struct BlacklistedPlayer
    {
        std::string Name;
        bool Autokick = false;
        bool Automute = false;
    };

private:
    void AddPlayerIntoBlacklistedPlayers(Core::Pointer playerStatusData);

private:
    Core::Logger& m_Logger;

    std::map<uint64_t, BlacklistedPlayer> m_BlacklistedPlayers;
    std::vector<uint64_t> m_BlacklistedPlayerIDs;
    std::string m_BlacklistedPlayersFilePath;
    bool m_BlacklistEnabled = true;
};