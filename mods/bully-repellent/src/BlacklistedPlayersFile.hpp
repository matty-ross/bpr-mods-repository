#pragma once


#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include "core/Path.hpp"
#include "core/Logger.hpp"


class BlacklistedPlayersFile
{
public:
    struct BlacklistedPlayer
    {
        uint64_t ID = -1;
        std::string Name;
        bool Autokick = false;
        bool Automute = false;
    };

public:
    BlacklistedPlayersFile(Core::Path configDirectoryPath, const Core::Logger& logger);

public:
    std::span<BlacklistedPlayer> GetBlacklistedPlayers();
    BlacklistedPlayer* FindBlacklistedPlayerByID(uint64_t blacklistedPlayerID);
    void AddBlacklistedPlayer(const BlacklistedPlayer& blacklistedPlayer);
    void RemoveBlacklistedPlayerByID(uint64_t blacklistedPlayerID);

    void Load();
    void Save() const;

private:
    static constexpr char k_Name[] = "blacklisted players";

private:
    Core::Path m_FilePath;

    std::vector<BlacklistedPlayer> m_BlacklistedPlayers;

    const Core::Logger& m_Logger;
};
