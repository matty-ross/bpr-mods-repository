#pragma once


#include <string>
#include <map>
#include <vector>

#include "core/Logger.h"


struct BlacklistedPlayer
{
    std::string Name;
    bool Autokick = false;
    bool Automute = false;
};


class BlacklistedPlayersFile
{
public:
    BlacklistedPlayersFile(const Core::Logger& logger, const std::string& filePath);

public:
    void OnRenderMenu();

    void Load();
    void Save();
    
    const std::map<uint64_t, BlacklistedPlayer>& GetBlacklistedPlayers() const;
    void AddBlacklistedPlayer(uint64_t blacklistedPlayerID, const BlacklistedPlayer& blacklistedPlayer);

    bool IsBlacklistEnabled() const;

private:
    const Core::Logger& m_Logger;

    std::string m_FilePath;
    
    std::map<uint64_t, BlacklistedPlayer> m_BlacklistedPlayers;
    std::vector<uint64_t> m_BlacklistedPlayerIDs;

    bool m_BlacklistEnabled = true;
};