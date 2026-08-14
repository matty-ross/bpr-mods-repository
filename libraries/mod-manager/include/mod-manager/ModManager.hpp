#pragma once


#include <Windows.h>

#include "core/Path.hpp"
#include "core/Logger.hpp"
#include "mod-manager/ModManagerApi.hpp"
#include "mod-manager/ModManagerConfigFile.hpp"
#include "mod-manager/HookManager.hpp"
#include "mod-manager/ImGuiManager.hpp"


class ModManager
{
private:
    ModManager();

public:
    ModManager(const ModManager&) = delete;
    ModManager(ModManager&&) = delete;

    ModManager& operator =(const ModManager&) = delete;
    ModManager& operator =(ModManager&&) = delete;

public:
    MOD_MANAGER_API static ModManager& Get();

public:
    MOD_MANAGER_API Core::Path GetConfigDirectory() const;

    MOD_MANAGER_API HookManager& GetHookManager();
    MOD_MANAGER_API ImGuiManager& GetImGuiManager();

    MOD_MANAGER_API bool CheckModVersion(const char* modVersion) const;

    void Load();
    void Unload();

private:
    void DeferredLoad();

    void RenderMenu();

private:
    static constexpr char k_Name[] = "Mod Manager";
    static constexpr char k_Version[] = "2.0.0";
    static constexpr char k_Author[] = "PISros0724 (Matty)";

    static ModManager s_Instance;

private:
    Core::Logger m_Logger;

    Core::Path m_ConfigDirectory;

    ModManagerConfigFile m_ModManagerConfigFile;

    HookManager m_HookManager;
    ImGuiManager m_ImGuiManager;

    HANDLE m_DeferredLoadThreadHandle = NULL;
};
