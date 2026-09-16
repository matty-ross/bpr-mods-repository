#include <cstdint>

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"

#include "BlacklistedPlayersFile.hpp"
#include "OnlinePlayers.hpp"


namespace BPR
{
    // BrnGui::GuiEventNetworkSelectedPlayerOption
    struct GuiOutEvent_SelectedPlayerOption
    {
        static constexpr int32_t ID = 283;

        int32_t Option;
        uint64_t PlayerID;
    };


    /*
        bool __thiscall CgsModule::VariableEventQueue<18432, 16>::AddEvent(
            const CgsModule::Event* lpEvent,
            int32_t liEventId,
            int32_t liEventSize
        )
    */
    static bool GuiOutEventQueue_AddGuiOutEvent(void* guiOutEventQueue, const void* guiOutEvent, int32_t guiOutEventID, int32_t guiOutEventSize)
    {
        bool result = false;

        __asm
        {
            push dword ptr [guiOutEventSize]
            push dword ptr [guiOutEventID]
            push dword ptr [guiOutEvent]
            mov ecx, dword ptr [guiOutEventQueue]

            mov eax, 0x00491DE0
            call eax

            mov byte ptr [result], al
        }

        return result;
    }
}


OnlinePlayers::OnlinePlayers(BlacklistedPlayersFile& blacklistedPlayersFile)
    :
    m_BlacklistedPlayersFile(blacklistedPlayersFile)
{
}

void OnlinePlayers::CheckPlayers(
    Core::Pointer guiOutEventQueue // CgsGui::GuiResourceModuleIO::InputBuffer::GuiEventQueue*
)
{
    if (!m_BlacklistEnabled)
    {
        return;
    }

    Core::Pointer guiCache = Core::Pointer(0x013FC8E0).deref().at(0x8E8430); // BrnGui::GuiCache*

    int32_t currentGameMode = guiCache.at(0xCF38).as<int32_t>();
    if (!(currentGameMode == 15 || currentGameMode == 16)) // BrnGameState::GameStateModuleIO::E_MODE_ONLINE_FREE_BURN_LOBBY || BrnGameState::GameStateModuleIO::E_MODE_ONLINE_SHOWTIME
    {
        return;
    }

    int32_t playersCount = guiCache.at(0xDE2C).as<int32_t>();
    for (int32_t i = 0; i < playersCount; ++i)
    {
        Core::Pointer playerStatusData = guiCache.at(0xDE38 + i * 0x138); // BrnNetwork::BrnNetworkModuleIO::InGamePlayerStatusData*
        uint64_t playerID = playerStatusData.at(0x110).as<uint64_t>();

        bool isLocalPlayer = playerStatusData.at(0x12D).as<bool>();
        if (isLocalPlayer)
        {
            continue;
        }

        const BlacklistedPlayersFile::BlacklistedPlayer* blacklistedPlayer = m_BlacklistedPlayersFile.FindBlacklistedPlayerByID(playerID);
        if (blacklistedPlayer == nullptr)
        {
            continue;
        }

        if (blacklistedPlayer->Autokick)
        {
            bool isLocalPlayerHost = guiCache.at(0xEA59).as<bool>();
            if (isLocalPlayerHost)
            {
                BPR::GuiOutEvent_SelectedPlayerOption guiOutEvent =
                {
                    .Option = 2,
                    .PlayerID = playerID,
                };
                BPR::GuiOutEventQueue_AddGuiOutEvent(guiOutEventQueue.GetPointer(), &guiOutEvent, guiOutEvent.ID, sizeof(guiOutEvent));
            }
        }
        if (blacklistedPlayer->Automute)
        {
            bool isMuted = playerStatusData.at(0x130).as<bool>();
            if (!isMuted)
            {
                BPR::GuiOutEvent_SelectedPlayerOption guiOutEvent =
                {
                    .Option = 3,
                    .PlayerID = playerID,
                };
                BPR::GuiOutEventQueue_AddGuiOutEvent(guiOutEventQueue.GetPointer(), &guiOutEvent, guiOutEvent.ID, sizeof(guiOutEvent));
            }
        }
    }
}

void OnlinePlayers::RenderMenu()
{
    if (ImGui::CollapsingHeader("Current Players"))
    {
        Core::Pointer guiCache = Core::Pointer(0x013FC8E0).deref().at(0x8E8430); // BrnGui::GuiCache*

        if (ImGui::BeginTable("##current-players", 2))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.8f);
            ImGui::TableSetupColumn("Blacklist", ImGuiTableColumnFlags_WidthStretch, 0.2f);
            ImGui::TableHeadersRow();

            bool isOnline = guiCache.at(0x7B00).as<bool>();
            if (isOnline)
            {
                int32_t playersCount = guiCache.at(0xDE2C).as<int32_t>();
                for (int32_t i = 0; i < playersCount; ++i)
                {
                    Core::Pointer playerStatusData = guiCache.at(0xDE38 + i * 0x138); // BrnNetwork::BrnNetworkModuleIO::InGamePlayerStatusData*
                    uint64_t playerID = playerStatusData.at(0x110).as<uint64_t>();

                    bool isLocalPlayer = playerStatusData.at(0x12D).as<bool>();
                    if (isLocalPlayer)
                    {
                        continue;
                    }

                    ImGui::PushID(playerStatusData.GetPointer());

                    ImGui::TableNextRow();
                    
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(playerStatusData.at(0xF0).as<char[25]>());
                    
                    ImGui::TableNextColumn();
                    bool alreadyOnBlacklist = m_BlacklistedPlayersFile.FindBlacklistedPlayerByID(playerID) != nullptr;
                    if (alreadyOnBlacklist)
                    {
                        ImGui::BeginDisabled();
                    }
                    if (ImGui::Button("Add##blacklisted-player"))
                    {
                        m_BlacklistedPlayersFile.AddBlacklistedPlayer(
                            BlacklistedPlayersFile::BlacklistedPlayer
                            {
                                .ID = playerID,
                                .Name = playerStatusData.at(0xF0).as<char[25]>(),
                                .Autokick = false,
                                .Automute = false,
                            }
                        );
                    }
                    if (alreadyOnBlacklist)
                    {
                        ImGui::EndDisabled();
                    }

                    ImGui::PopID();
                }
            }

            ImGui::EndTable();
        }
    }

    if (ImGui::CollapsingHeader("Blacklisted Players"))
    {
        ImGui::Checkbox("Blacklist Enabled", &m_BlacklistEnabled);

        if (ImGui::Button("Save##blacklisted-players-file"))
        {
            m_BlacklistedPlayersFile.Save();
        }

        ImGui::SameLine();

        if (ImGui::Button("Load##blacklisted-players-file"))
        {
            m_BlacklistedPlayersFile.Load();
        }

        ImGui::Separator();

        static ImGuiTextFilter blacklistedPlayerFilter;
        blacklistedPlayerFilter.Draw("Filter##blacklisted-player");

        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        if (ImGui::BeginTable("##blacklisted-players", 4, ImGuiTableFlags_ScrollY, ImVec2(0.0f, max(availableSize.y, 40.0f))))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.5f);
            ImGui::TableSetupColumn("Autokick", ImGuiTableColumnFlags_WidthStretch, 0.15f);
            ImGui::TableSetupColumn("Automute", ImGuiTableColumnFlags_WidthStretch, 0.15f);
            ImGui::TableSetupColumn("Blacklist", ImGuiTableColumnFlags_WidthStretch, 0.2f);
            ImGui::TableHeadersRow();

            uint64_t blacklistedPlayerIdToRemove = -1;

            for (BlacklistedPlayersFile::BlacklistedPlayer& blacklistedPlayer : m_BlacklistedPlayersFile.GetBlacklistedPlayers())
            {
                if (blacklistedPlayerFilter.PassFilter(blacklistedPlayer.Name.c_str()))
                {
                    ImGui::PushID(&blacklistedPlayer);

                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextUnformatted(blacklistedPlayer.Name.c_str());

                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##autokick", &blacklistedPlayer.Autokick);

                    ImGui::TableNextColumn();
                    ImGui::Checkbox("##automute", &blacklistedPlayer.Automute);

                    ImGui::TableNextColumn();
                    if (ImGui::Button("Remove##blacklisted-player"))
                    {
                        blacklistedPlayerIdToRemove = blacklistedPlayer.ID;
                    }

                    ImGui::PopID();
                }
            }

            if (blacklistedPlayerIdToRemove != -1)
            {
                m_BlacklistedPlayersFile.RemoveBlacklistedPlayerByID(blacklistedPlayerIdToRemove);
            }

            ImGui::EndTable();
        }
    }
}
