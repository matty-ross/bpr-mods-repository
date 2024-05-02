#pragma once


#include <string>
#include <vector>

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
    CustomParametersFile(const std::string& filePath, const Core::Logger& logger);

public:
    void Load();
    void Save() const;

    std::vector<CustomParameters>& GetCustomParameters();

private:
    std::string m_FilePath;

    std::vector<CustomParameters> m_CustomParameters;
    bool m_Valid = false;

    const Core::Logger& m_Logger;
};
