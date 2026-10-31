module;
#include <cstdlib>
#include <fstream>
#include <string>
#include <windows.h>

export module Config;
import std;
import Console;

export struct AppConfig {
    std::wstring targetFolder = L"";
    int autoRunMode = 0;
    int customTimeHours = 1;
};

inline std::filesystem::path GetConfigPath() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    return std::filesystem::path(exePath).parent_path() / L"config.ini";
}

export void SaveConfig(const AppConfig& cfg) {
    std::ofstream file(GetConfigPath());
    if (!file) return;
    std::string pathStr = ToUtf8(cfg.targetFolder);
    file << "TargetFolder=" << pathStr << "\n";
    file << "AutoRunMode=" << cfg.autoRunMode << "\n";
    file << "CustomTime=" << cfg.customTimeHours << "\n";
}

export AppConfig LoadConfig() {
    AppConfig cfg;
    std::ifstream file(GetConfigPath());
    if (!file) return cfg;

    std::string line;
    while (std::getline(file, line)) {
        if (line.starts_with("\xEF\xBB\xBF")) {
            line = line.substr(3);
        }

        if (line.starts_with("TargetFolder=")) {
            std::string val = line.substr(13);
            int size = MultiByteToWideChar(CP_UTF8, 0, val.c_str(), -1, nullptr, 0);
            if (size > 0) {
                cfg.targetFolder.resize(size - 1);
                MultiByteToWideChar(CP_UTF8, 0, val.c_str(), -1, cfg.targetFolder.data(), size);
            }
        }
        else if (line.starts_with("AutoRunMode=")) {
            try { cfg.autoRunMode = std::stoi(line.substr(12)); }
            catch (...) { cfg.autoRunMode = 0; }
        }
        else if (line.starts_with("CustomTime=")) {
            try { cfg.customTimeHours = std::stoi(line.substr(11)); }
            catch (...) { cfg.customTimeHours = 1; }
        }
    }
    return cfg;
}

inline std::wstring GetTaskName() { return L"DownloadsSorter_AutoRun"; }

export void DeleteScheduledTask() {
    std::wstring cmd = L"schtasks /Delete /TN \"" + GetTaskName() + L"\" /F >nul 2>&1";
    _wsystem(cmd.c_str());
}

export void CreateLogonTask() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    std::wstring cmd = L"schtasks /Create /TN \"" + GetTaskName() +
        L"\" /TR \"\\\"" + exePath + L"\\\" --silent\" /SC ONLOGON /F >nul 2>&1";
    _wsystem(cmd.c_str());
}

export void CreateHourlyTask(int hours) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    std::wstring cmd = L"schtasks /Create /TN \"" + GetTaskName() +
        L"\" /TR \"\\\"" + exePath + L"\\\" --silent\" /SC HOURLY /MO " + std::to_wstring(hours) +
        L" /F >nul 2>&1";
    _wsystem(cmd.c_str());
}

export void ApplyAutoRunSettings(const AppConfig& cfg) {
    DeleteScheduledTask();
    if (cfg.autoRunMode == 1) CreateLogonTask();
    else if (cfg.autoRunMode == 2 && cfg.customTimeHours > 0) CreateHourlyTask(cfg.customTimeHours);
}