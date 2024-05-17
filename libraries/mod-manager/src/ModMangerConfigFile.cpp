#include "ModMangerConfigFile.hpp"


ModMangerConfigFile::ModMangerConfigFile(const std::string& filePath, const Core::Logger& logger)
    :
    m_FilePath(filePath),
    m_Logger(logger)
{
}

void ModMangerConfigFile::Load()
{
    // TODO: implement this method
}

void ModMangerConfigFile::Save() const
{
    // TODO: implement this method
}

Config& ModMangerConfigFile::GetConfig()
{
    return m_Config;
}
