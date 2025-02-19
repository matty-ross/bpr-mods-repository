#pragma once


#include <string>
#include <vector>

#include "core/Logger.hpp"


struct BlacklistedPlayer
{
    uint64_t ID = 0;
    std::string Name;
    bool Autokick = false;
    bool Automute = false;
};


class BlacklistedPlayersFile
{
public:
    BlacklistedPlayersFile(const char* filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;
    
    std::vector<BlacklistedPlayer>& GetBlacklistedPlayers();
    BlacklistedPlayer* GetBlacklistedPlayer(uint64_t blacklistedPlayerID);

private:
    const char* m_FilePath;
    const Core::Logger& m_Logger;
    
    std::vector<BlacklistedPlayer> m_BlacklistedPlayers;
};
