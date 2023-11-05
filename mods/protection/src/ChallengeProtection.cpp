#include "ChallengeProtection.h"

#include "imgui/imgui.h"

#include "core/Pointer.h"


ChallengeProtection::ChallengeProtection(ChallengesFile& challengesFile)
    :
    m_ChallengesFile(challengesFile)
{
}

void ChallengeProtection::OnFreeburnChallengeMessagePack(void* freeburnChallengeMessage)
{
    if (!m_ChallengeProtectionEnabled)
    {
        return;
    }

    uint64_t& challengeID = Core::Pointer(freeburnChallengeMessage).at(0x38).as<uint64_t>();
    challengeID = HandleChallengeID(challengeID);
}

void ChallengeProtection::OnFreeburnChallengeMessageUnpack(void* freeburnChallengeMessage)
{
    if (!m_ChallengeProtectionEnabled)
    {
        return;
    }

    uint64_t& challengeID = Core::Pointer(freeburnChallengeMessage).at(0x38).as<uint64_t>();
    challengeID = HandleChallengeID(challengeID);
}

void ChallengeProtection::OnRenderMenu()
{
    if (ImGui::CollapsingHeader("Challenge Protection"))
    {
        ImGui::Checkbox("Challenge Protection Enabled", &m_ChallengeProtectionEnabled);

        if (ImGui::Button("Save"))
        {
            m_ChallengesFile.Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            m_ChallengesFile.Load();
        }
        
        if (ImGui::BeginCombo("Fallback Challenge", m_ChallengesFile.GetFallbackChallengeID()->String))
        {
            for (const ChallengeID& vanillaChallengeID : k_VanillaChallengeIDs)
            {
                bool selected = m_ChallengesFile.GetFallbackChallengeID()->Number == vanillaChallengeID.Number;
                if (ImGui::Selectable(vanillaChallengeID.String, selected))
                {
                    m_ChallengesFile.SetFallbackChallengeID(&vanillaChallengeID);
                }
                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginTable("##challenges-table", 2))
        {
            ImGui::TableSetupColumn("New Challenge");
            ImGui::TableSetupColumn("Replacement Challenge");
            ImGui::TableHeadersRow();

            for (uint64_t challengeID : m_ChallengesFile.GetChallengeIDs())
            {
                Challenge& challenge = *(m_ChallengesFile.GetChallenge(challengeID));

                ImGui::PushID(&challenge);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(challenge.NewID.String);

                ImGui::TableSetColumnIndex(1);
                if (ImGui::BeginCombo("##replacement-challenge-combo", challenge.ReplacementID->String))
                {
                    for (const ChallengeID& vanillaChallengeID : k_VanillaChallengeIDs)
                    {
                        bool selected = challenge.ReplacementID->Number == vanillaChallengeID.Number;
                        if (ImGui::Selectable(vanillaChallengeID.String, selected))
                        {
                            challenge.ReplacementID = &vanillaChallengeID;
                        }
                        if (selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }
}

void ChallengeProtection::AddNonVanillaChallengeIDsToChallengesFile()
{
    Core::Pointer challengeList = Core::Pointer(0x013FC8E0).deref().at(0x690B70);

    int32_t challengesCount = challengeList.at(0x32E0).as<int32_t>();
    for (int32_t i = 0; i < challengesCount; ++i)
    {
        Core::Pointer challengeSlot = challengeList.at(0x400 + i * 0xC);
        int32_t listIndex = challengeSlot.at(0x4).as<int32_t>();
        int32_t entryIndex = challengeSlot.at(0x8).as<int32_t>();

        Core::Pointer list = challengeList.at(0x0 + listIndex * 0x20).as<void*>();
        Core::Pointer entry = list.at(0x4).deref().at(entryIndex * 0xD8);

        uint64_t challengeID = entry.at(0xC0).as<uint64_t>();
        bool isVanilla = GetVanillaChallengeID(challengeID) != nullptr;
        bool isInFile = m_ChallengesFile.GetChallenge(challengeID) != nullptr;
        if (!isVanilla && !isInFile)
        {
            ChallengeID nonVanillaChallengeID = {};
            nonVanillaChallengeID.Number = challengeID;
            sprintf_s(nonVanillaChallengeID.String, "%llu", challengeID);

            m_ChallengesFile.AddChallenge(
                {
                    .NewID = nonVanillaChallengeID,
                    .ReplacementID = m_ChallengesFile.GetFallbackChallengeID(),
                }
            );
        }
    }
}

uint64_t ChallengeProtection::HandleChallengeID(uint64_t challengeID)
{
    bool isVanilla = GetVanillaChallengeID(challengeID) != nullptr;
    if (isVanilla)
    {
        return challengeID;
    }

    Challenge* challenge = m_ChallengesFile.GetChallenge(challengeID);
    if (challenge == nullptr)
    {
        return m_ChallengesFile.GetFallbackChallengeID()->Number;
    }

    return challenge->ReplacementID->Number;
}