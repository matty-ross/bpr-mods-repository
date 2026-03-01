#pragma once


#include "core/Logger.hpp"


class RemovedStuff
{
public:
    RemovedStuff(const Core::Logger& logger);

public:
    void Load();
    void Unload();

private:
    const Core::Logger& m_Logger;
};
