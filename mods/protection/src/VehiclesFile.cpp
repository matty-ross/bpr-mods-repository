#include "VehiclesFile.h"


VehiclesFile::VehiclesFile(const Core::Logger& logger, const std::string& filePath)
    :
    m_Logger(logger),
    m_FilePath(filePath)
{
}

void VehiclesFile::Load()
{
}

void VehiclesFile::Save()
{
}

uint64_t VehiclesFile::GetFallbackVehicleID() const
{
    return m_FallbackVehicleID;
}

void VehiclesFile::SetFallbackVehicleID(uint64_t fallbackVehicleID)
{
    m_FallbackVehicleID = fallbackVehicleID;
}

std::map<uint64_t, uint64_t>& VehiclesFile::GetVehicles()
{
    return m_Vehicles;
}

const std::vector<uint64_t>& VehiclesFile::GetVehicleIDs() const
{
    return m_VehicleIDs;
}

void VehiclesFile::AddVehicle(uint64_t vehicleID, uint64_t vehicleReplacementID)
{
    m_Vehicles[vehicleID] = vehicleReplacementID;
    m_VehicleIDs.push_back(vehicleID);
}