#include "Misc.hpp"

#include "vendor/imgui.hpp"

#include "bpr-sdk/GameActions.hpp"
#include "core/Pointer.hpp"


namespace BPR
{
    // void __thiscall BrnGui::RaceMainHudState::OnEnter()
    static void RaceMainHudState_OnEnter()
    {
        __asm
        {
            mov ecx, dword ptr ds:[0x013FC8E0]
            mov ecx, dword ptr [ecx + 0x7FABBC]
            
            mov eax, 0x00ABC890
            call eax
        }
    }

    // void __thiscall BrnGui::RaceMainHudState::OnLeave()
    static void RaceMainHudState_OnLeave()
    {
        __asm
        {
            mov ecx, dword ptr ds:[0x013FC8E0]
            mov ecx, dword ptr [ecx + 0x7FABBC]
            
            mov eax, 0x00ABC4D0
            call eax
        }
    }
}


void Misc::OnPreWorldUpdate(
    void* gameEventQueue, // BrnGameState::GameStateModuleIO::GameEventQueue*
    void* gameActionQueue // BrnGameState::GameStateModuleIO::BaseGameActionQueue<13312>*
)
{
    if (m_PauseSimulation)
    {
        BPR::GameAction_PauseSimulation gameAction = {};
        BPR::GameActionQueue_AddGameAction(gameActionQueue, &gameAction, gameAction.ID, sizeof(gameAction));

        m_PauseSimulation = false;
    }

    if (m_UnpauseSimulation)
    {
        BPR::GameAction_UnpauseSimulation gameAction = {};
        BPR::GameActionQueue_AddGameAction(gameActionQueue, &gameAction, gameAction.ID, sizeof(gameAction));

        m_UnpauseSimulation = false;
    }
}

void Misc::OnUpdateHudFlow()
{
    if (m_HideHUD)
    {
        BPR::RaceMainHudState_OnLeave();
        m_HideHUD = false;
    }
    
    if (m_ShowHUD)
    {
        BPR::RaceMainHudState_OnEnter();
        m_ShowHUD = false;
    }
}

void Misc::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Misc"))
    {
        Core::Pointer gameModule = Core::Pointer(0x013FC8E0).deref(); // BrnGame::BrnGameModule*

        {
            bool isOnline = gameModule.at(0xB6D415).as<bool>();
            bool& simulationPaused = gameModule.at(0xB6D3FF).as<bool>();

            if (isOnline)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::Checkbox("Simulation Paused", &simulationPaused))
            {
                if (simulationPaused)
                {
                    m_PauseSimulation = true;
                }
                else
                {
                    m_UnpauseSimulation = true;
                }
            }

            if (isOnline)
            {
                ImGui::EndDisabled();
            }
        }

        {
            bool& inHUD = gameModule.at(0x7FABBC).deref().at(0x14C).as<bool>();

            if (ImGui::Checkbox("HUD Visible", &inHUD))
            {
                if (inHUD)
                {
                    m_ShowHUD = true;
                }
                else
                {
                    m_HideHUD = true;
                }
            }
        }

        {
            bool& crashWhenFatal = gameModule.at(0x1E1364).as<bool>();
            
            if (ImGui::Checkbox("Crash when Off-Road", &crashWhenFatal))
            {
                Core::Pointer(0x0140AA9F).as<bool>() = crashWhenFatal;
            }
        }
    }
}
