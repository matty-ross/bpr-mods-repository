#include "ChallengeProtection.h"

#include "imgui.h"

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

        static ImGuiTextFilter challengeTitleComboFilter;
        challengeTitleComboFilter.Draw("Challenge Combo Filter");

        if (ImGui::Button("Save"))
        {
            m_ChallengesFile.Save();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            m_ChallengesFile.Load();
        }
        
        if (ImGui::BeginCombo("Fallback Challenge", m_ChallengesFile.GetFallbackChallenge()->Title))
        {
            for (const VanillaChallenge& vanillaChallenge : k_VanillaChallenges)
            {
                if (challengeTitleComboFilter.PassFilter(vanillaChallenge.Title))
                {
                    bool selected = m_ChallengesFile.GetFallbackChallenge()->ID == vanillaChallenge.ID;
                    if (ImGui::Selectable(vanillaChallenge.Title, selected))
                    {
                        m_ChallengesFile.SetFallbackChallenge(&vanillaChallenge);
                    }
                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginTable("##challenges-table", 2))
        {
            ImGui::TableSetupColumn("Challenge", ImGuiTableColumnFlags_WidthStretch, 0.4f);
            ImGui::TableSetupColumn("Replacement Challenge", ImGuiTableColumnFlags_WidthStretch, 0.6f);
            ImGui::TableHeadersRow();
            for (Challenge& challenge : m_ChallengesFile.GetChallenges())
            {
                ImGui::PushID(&challenge);
                ImGui::TableNextRow();
                {
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(challenge.Title.c_str());
                }
                {
                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(-35.0f);
                    if (ImGui::BeginCombo("##replacement-challenge-combo", challenge.Replacement->Title))
                    {
                        for (const VanillaChallenge& vanillaChallenge : k_VanillaChallenges)
                        {
                            if (challengeTitleComboFilter.PassFilter(vanillaChallenge.Title))
                            {
                                bool selected = challenge.Replacement->ID == vanillaChallenge.ID;
                                if (ImGui::Selectable(vanillaChallenge.Title, selected))
                                {
                                    challenge.Replacement = &vanillaChallenge;
                                }
                                if (selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                        }
                        ImGui::EndCombo();
                    }
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }
}

void ChallengeProtection::AddNonVanillaChallengesToChallengesFile()
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
        bool isVanilla = GetVanillaChallenge(challengeID) != nullptr;
        bool isInFile = m_ChallengesFile.GetChallenge(challengeID) != nullptr;
        if (!isVanilla && !isInFile)
        {
            m_ChallengesFile.GetChallenges().push_back(
                Challenge
                {
                    .ID          = challengeID,
                    .Title       = entry.at(0xB0).as<char[16]>(),
                    .Replacement = m_ChallengesFile.GetFallbackChallenge(),
                }
            );
        }
    }
}

uint64_t ChallengeProtection::HandleChallengeID(uint64_t challengeID) const
{
    bool isVanilla = GetVanillaChallenge(challengeID) != nullptr;
    if (isVanilla)
    {
        return challengeID;
    }

    Challenge* challenge = m_ChallengesFile.GetChallenge(challengeID);
    if (challenge != nullptr)
    {
        return challenge->Replacement->ID;
    }

    return m_ChallengesFile.GetFallbackChallenge()->ID;
}