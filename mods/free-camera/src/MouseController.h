#pragma once


#include <Windows.h>


class MouseController
{
public:
    void OnWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

    bool IsRotating() const;
    float GetRotationDeltaX();
    float GetRotationDeltaY();
    
    float GetTranslationDeltaZ();

private:
    float m_CurrentPositionX = 0.0f;
    float m_CurrentPositionY = 0.0f;
    float m_PreviousPositionX = 0.0f;
    float m_PreviousPositionY = 0.0f;
 
    float m_WheelDelta = 0.0f;
    
    bool m_LeftButtonDown = false;
};