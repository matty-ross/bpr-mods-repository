#include "MouseController.h"

#include <Windowsx.h>


void MouseController::OnWindowMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_LBUTTONDOWN:
        {
            m_LeftButtonDown = true;   
        }
        break;

    case WM_LBUTTONUP:
        {
            m_LeftButtonDown = false;
        }
        break;

    case WM_MOUSEMOVE:
        {
            m_PreviousPositionX = m_CurrentPositionX;
            m_PreviousPositionY = m_CurrentPositionY;
            m_CurrentPositionX = static_cast<float>(GET_X_LPARAM(lParam));
            m_CurrentPositionY = static_cast<float>(GET_Y_LPARAM(lParam));
        }
        break;

    case WM_MOUSEWHEEL:
        {
            m_WheelDelta += static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
        }
        break;
    }
}

bool MouseController::IsRotating() const
{
    return m_LeftButtonDown;
}

float MouseController::GetRotationDeltaX() const
{
    return m_CurrentPositionX - m_PreviousPositionX;
}

float MouseController::GetRotationDeltaY() const
{
    return m_CurrentPositionY - m_PreviousPositionY;
}

float MouseController::GetTranslationDeltaZ() const
{
    return m_WheelDelta;
}