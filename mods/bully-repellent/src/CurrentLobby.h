#pragma once


#include <string>
#include <vector>

#include "core/Logger.h"
#include "core/Pointer.h"


class CurrentLobby
{
public:
    CurrentLobby(Core::Logger& logger, const std::string& blacklistedPlayersFilePath);

public:
    //void OnUpdate();
    void OnRenderMenu();
    
    void LoadBlacklistedPlayers();
    void SaveBlacklistedPlayers();

private:
    struct BlacklistedPlayer
    {
        uint64_t ID;
        std::string Name;
        bool Autokick;
        bool Automute;
    };

private:
    void AddPlayerIntoBlacklistedPlayers(Core::Pointer playerStatus);

private:
    Core::Logger& m_Logger;

    std::vector<BlacklistedPlayer> m_BlacklistedPlayers;
    std::string m_BlacklistedPlayersFilePath;
};