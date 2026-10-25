#pragma once


#include <vendor/imgui.hpp>

#include <core/Pointer.hpp>
#include <core/Path.hpp>
#include <core/Logger.hpp>
#include <bpr/Types.hpp>

#include "DashboardConfigFile.hpp"
#include "DashboardTexture.hpp"


class DashboardHud
{
public:
    DashboardHud(DashboardConfigFile& dashboardConfigFile, const Core::Path& assetsDirectoryPath, const Core::Logger& logger);

public:
    void Load();

    void RenderMenu();
    void RenderOverlay();

    void LoadTexture();
    void LoadFont();

private:
    void RenderTextureSegment(const ImVec2& position, DashboardTexture::TextureSegment textureSegment, bool useColor = true) const;
    void RenderText(const ImVec2& position, const char* text, float size) const;
    void RenderNeedle(const ImVec2& position, float value, float minValue, float maxValue) const;

    void AddDistanceDriven(Core::Pointer progressionManager, float distance, BPR::VehicleType vehicleType);

private:
    static void Hook_AddDistanceDriven();

private:
    DashboardConfigFile& m_DashboardConfigFile;

    const Core::Path& m_AssetsDirectoryPath;
    DashboardTexture m_DashboardTexture;
    ImFont* m_Font = nullptr;

    float m_TripMeter = 0.0f;
    float m_Odometer = 0.0f;

    const Core::Logger& m_Logger;
};
