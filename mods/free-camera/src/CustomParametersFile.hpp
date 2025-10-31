#pragma once


#include <string>
#include <vector>

#include "core/Path.hpp"
#include "core/Logger.hpp"


struct CustomParameters
{
    std::string Name;
    float PitchSpring;
    float YawSpring;
    float PivotY;
    float PivotZ;
    float PivotZOffset;
    float FOV;
    float InFrontFOVMax;
    float FrontInAmount;
    float DriftYawSpring;
    float BoostFOVZoomCompensation;
    float DownAngle;
    float DropFactor;
};


class CustomParametersFile
{
public:
    CustomParametersFile(Core::Path directory, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    std::vector<CustomParameters>& GetCustomParameters();

private:
    Core::Path m_FilePath;
    const Core::Logger& m_Logger;

    std::vector<CustomParameters> m_CustomParameters;
};
