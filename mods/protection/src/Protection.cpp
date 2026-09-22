#include "Protection.hpp"

#include "vendor/imgui.hpp"

#include "core/Pointer.hpp"
#include "mod-manager/ModManager.hpp"


Protection Protection::s_Instance;


Protection::Protection()
    :
    m_Logger(k_Name),
    m_ConfigDirectoryPath(ModManager::Get().GetConfigDirectoryPath().Append(k_ConfigDirectoryPath)),
    m_VehiclesFile(m_ConfigDirectoryPath, m_Logger),
    m_ChallengesFile(m_ConfigDirectoryPath, m_Logger),
    m_VehicleProtection(m_VehiclesFile, m_Logger),
    m_ChallengeProtection(m_ChallengesFile, m_Logger)
{
}

Protection& Protection::Get()
{
    return s_Instance;
}

void Protection::Load()
{
    try
    {
        if (!ModManager::Get().CheckVersion(k_Version))
        {
            throw std::exception("Mod Manager and Mod versions mismatch.");
        }

        if (!m_ConfigDirectoryPath.Exists())
        {
            m_ConfigDirectoryPath.CreateDirectoryTree();
            m_Logger.Info("Created config directory. path: '%s'", m_ConfigDirectoryPath.GetPath());
        }

        m_VehiclesFile.Load();
        m_ChallengesFile.Load();

        m_VehicleProtection.Load();
        m_ChallengeProtection.Load();

        ModManager::Get().GetImGuiManager().AddMenu([]() { s_Instance.RenderMenu(); });

        auto deferredLoadThreadProc = [](LPVOID) -> DWORD
        {
            s_Instance.DeferredLoad();

            return 0;
        };
        HANDLE deferredLoadThreadHandle = CreateThread(nullptr, 0, deferredLoadThreadProc, nullptr, 0, nullptr);
        if (deferredLoadThreadHandle != NULL)
        {
            CloseHandle(deferredLoadThreadHandle);
        }
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void Protection::Unload()
{
    try
    {
        m_VehiclesFile.Save();
        m_ChallengesFile.Save();
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void Protection::DeferredLoad()
{
    try
    {
        while (true)
        {
            Core::Pointer gameModule = 0x013FC8E0;
            if (gameModule.as<void*>() != nullptr)
            {
                int32_t gameUpdateStage = gameModule.deref().at(0xB6D464).as<int32_t>();
                if (gameUpdateStage == 1) // BrnGame::BrnGameModule::E_GAMEUPDATESTAGE_MAIN
                {
                    break;
                }
            }

            Sleep(1000);
        }

        m_VehicleProtection.AddNonVanillaVehiclesToVehiclesFile();
        m_ChallengeProtection.AddNonVanillaChallengesToChallengesFile();
    }
    catch (const std::exception& ex)
    {
        m_Logger.Error("%s", ex.what());
        MessageBoxA(NULL, ex.what(), k_Name, MB_ICONERROR);
    }
}

void Protection::RenderMenu()
{
    if (ImGui::Begin(k_Name, nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.0f);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Version     %s", k_Version);
        ImGui::Text("Author      %s", k_Author);
        ImGui::Text("Framerate   %.1f", io.Framerate);

        m_VehicleProtection.RenderMenu();
        m_ChallengeProtection.RenderMenu();

        ImGui::PopItemWidth();
    }
    ImGui::End();
}
