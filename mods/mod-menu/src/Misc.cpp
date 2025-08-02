#include "Misc.hpp"

#include "vendor/imgui.hpp"

#include "bpr-sdk/GameActions.hpp"
#include "core/Pointer.hpp"


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

            if (ImGui::Checkbox("Pause Simulation", &simulationPaused))
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
            ImGui::Checkbox("Crash when Off-Road", &gameModule.at(0x1E1364).as<bool>());
        }
    }
}
