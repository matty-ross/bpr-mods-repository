#pragma once


#include <string>
#include <vector>

#include "core/Logger.h"


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
    CustomParametersFile(const Core::Logger& logger, const std::string& filePath);

public:
    void Load();
    void Save() const;

    const std::vector<CustomParameters>& GetCustomParameters() const;
    void AddCustomParameters(const CustomParameters& customParameters);

private:
    const Core::Logger& m_Logger;

    std::string m_FilePath;

    std::vector<CustomParameters> m_CustomParameters;

    bool m_Valid = false;
};