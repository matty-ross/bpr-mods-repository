#pragma once


#include <Windows.h>
#include <DirectXMath.h>

#include "core/Pointer.hpp"


class CurrentCamera
{
public:
    CurrentCamera();

public:
    void OnArbitratorUpdate(Core::Pointer camera, Core::Pointer arbStateSharedInfo);
    
    void OnRenderMenu();
    
    void OnMouseInput(const RAWMOUSE& mouse);

private:
    template <typename T>
    struct Property
    {
        ptrdiff_t Offset;
        T Value = {};
        bool Override = false;

        Property(ptrdiff_t offset)
        {
            Offset = offset;
        }
    };

    struct Properties
    {
        Property<float> FOV;
        Property<float> SimulationTimeScale;
        Property<float> CameraLag;
        Property<float> BlackBarAmount;
        Property<float> MotionBlurVehicle;
        Property<float> MotionBlurWorld;
        Property<float> Blurriness;
        bool ResetAll = false;
    };

    struct Transformation
    {
        DirectX::XMFLOAT3A Rotation = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3A RotationDelta = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3A Translation = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3A TranslationDelta = { 0.0f, 0.0f, 0.0f };
        bool Init = false;
        bool Override = false;
    };

    struct Effect
    {
        const char* Name = nullptr;
        float BlendAmount = 0.0f;
        bool Play = false;
        bool Stop = false;
    };

private:
    Properties m_Properties;
    Transformation m_Transformation;
    Effect m_Effect;
};
