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
    BlacklistedPlayersFile(const std::string& filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;
    
    const std::vector<uint64_t>& GetBlacklistedPlayerIDs() const;
    BlacklistedPlayer* GetBlacklistedPlayer(uint64_t blacklistedPlayerID);
    void AddBlacklistedPlayer(uint64_t blacklistedPlayerID, const BlacklistedPlayer& blacklistedPlayer);

private:
    std::string m_FilePath;
    const Core::Logger& m_Logger;
    
    std::map<uint64_t, BlacklistedPlayer> m_BlacklistedPlayers;
    std::vector<uint64_t> m_BlacklistedPlayerIDs;

    bool m_Valid = false;
};