#include "QualityOfLife.hpp"

#include "mod-manager/ModManager.hpp"


static constexpr char k_ModName[]      = "Quality of Life";
static constexpr char k_ModVersion[]   = "1.3.0";
static constexpr char k_ModAuthor[]    = "PISros0724 (Matty)";
static constexpr char k_ModDirectory[] = "quality-of-life\\";


QualityOfLife QualityOfLife::s_Instance;


QualityOfLife::QualityOfLife()
    :
    m_ConfigDirectory(ModManager::Get().GetConfigDirectory().Append(k_ModDirectory)),
    m_Logger(k_ModName),
    m_BugFixes(m_Logger)
{
}

QualityOfLife& QualityOfLife::Get()
{
    return s_Instance;
}

void QualityOfLife::OnProcessAttach()
{
    PTHREAD_START_ROUTINE loadThreadProc = [](LPVOID lpThreadParameter) -> DWORD
    {
        s_Instance.Load();
        return 0;
    };
    m_LoadThreadHandle = CreateThread(nullptr, 0, loadThreadProc, nullptr, 0, nullptr);
}

void QualityOfLife::OnProcessDetach()
{
    Unload();
    CloseHandle(m_LoadThreadHandle);
}

void QualityOfLife::Load()
{
    try
    {
        m_Logger.Info("Loading...");

        // Check mod version.
        {
            m_Logger.Info("Checking mod version...");

            if (!ModManager::Get().CheckModVersion(k_ModVersion))
            {
                throw std::exception("Mod Manager and Mod versions mismatch.");
            }

            m_Logger.Info("Checked mod version.");
        }

        // Create mod config directory.
        {
            m_Logger.Info("Creating mod config directory '%s' ...", m_ConfigDirectory.GetPath());

            m_ConfigDirectory.Create();

            m_Logger.Info("Created mod config directory.");
        }

        // Load bug fixes.
        {
            m_Logger.Info("Loading bug fixes...");
            
            m_BugFixes.Load();

            m_Logger.Info("Loaded bug fixes.");
        }

        m_Logger.Info("Loaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
    }
}

void QualityOfLife::Unload()
{
    try
    {
        m_Logger.Info("Unloading...");

        // Unload bug fixes.
        {
            m_Logger.Info("Unloading bug fixes...");

            m_BugFixes.Unload();

            m_Logger.Info("Unloaded bug fixes.");
        }

        m_Logger.Info("Unloaded.");
    }
    catch (const std::exception& e)
    {
        m_Logger.Error("%s", e.what());
        MessageBoxA(NULL, e.what(), k_ModName, MB_ICONERROR);
    }
}
