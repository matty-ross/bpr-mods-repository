#pragma once


#include <string>
#include <array>
#include <vector>

#include "core/Path.hpp"
#include "core/Logger.hpp"


struct CustomColor
{
    std::string Name;
    std::array<float, 3> PaintColor;
    std::array<float, 3> PaintColorIntensity;
    std::array<float, 3> PearlColor;
    std::array<float, 3> PearlColorIntensity;
};


class CustomColorsFile
{
public:
    CustomColorsFile(Core::Path directory, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    std::vector<CustomColor>& GetCustomColors();

private:
    Core::Path m_FilePath;
    const Core::Logger& m_Logger;

    std::vector<CustomColor> m_CustomColors;
};
