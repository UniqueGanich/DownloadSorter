#include "Config.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

fs::path GetConfigPath() {
#ifdef _WIN32
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    return fs::path(exePath).parent_path() / L"config.ini";
#else
    const char* home = std::getenv("HOME");
    fs::path configDir = home ? fs::path(home) / ".config" / "DownloadsSorter" : fs::current_path();
    fs::create_directories(configDir);
    return configDir / "config.ini";
#endif
}

void SaveConfig(const AppConfig& cfg) {
    std::ofstream file(GetConfigPath());
    if (!file) return;
    file << "TargetFolder=" << cfg.targetFolder.string() << "\n";
    file << "AutoRunMode=" << cfg.autoRunMode << "\n";
    file << "CustomTime=" << cfg.customTimeHours << "\n";
}

AppConfig LoadConfig() {
    AppConfig cfg;
    std::ifstream file(GetConfigPath());
    if (!file) return cfg;
    std::string line;
    while (std::getline(file, line)) {
        if (line.starts_with("\xEF\xBB\xBF")) line = line.substr(3);
        if (line.starts_with("TargetFolder=")) {
            cfg.targetFolder = line.substr(13);
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

void ApplyAutoRunSettings(const AppConfig& cfg) {
#ifdef _WIN32
    std::string taskName = "DownloadsSorter_AutoRun";
    std::string deleteCmd = "schtasks /Delete /TN \"" + taskName + "\" /F >nul 2>&1";
    system(deleteCmd.c_str());

    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::wstring wexePath(exePath);

    if (cfg.autoRunMode == 1) {
        std::wstring wcmd = L"schtasks /Create /TN \"" + std::wstring(taskName.begin(), taskName.end()) +
            L"\" /TR \"\\\"" + wexePath + L"\\\" --silent\" /SC ONLOGON /F >nul 2>&1";
        _wsystem(wcmd.c_str());
    }
    else if (cfg.autoRunMode == 2 && cfg.customTimeHours > 0) {
        std::wstring wcmd = L"schtasks /Create /TN \"" + std::wstring(taskName.begin(), taskName.end()) +
            L"\" /TR \"\\\"" + wexePath + L"\\\" --silent\" /SC HOURLY /MO " + std::to_wstring(cfg.customTimeHours) + L" /F >nul 2>&1";
        _wsystem(wcmd.c_str());
    }
#else
    std::error_code ec;
    std::string exePath = fs::canonical("/proc/self/exe", ec).string();
    if (ec) return;

    system("crontab -l | grep -v 'DownloadsSorter' | crontab -");

    if (cfg.autoRunMode == 1) {
        std::string cmd = "@reboot " + exePath + " --silent";
        system(("(crontab -l ; echo \"" + cmd + "\") | crontab -").c_str());
    }
    else if (cfg.autoRunMode == 2 && cfg.customTimeHours > 0) {
        std::string cmd = "0 */" + std::to_string(cfg.customTimeHours) + " * * * " + exePath + " --silent";
        system(("(crontab -l ; echo \"" + cmd + "\") | crontab -").c_str());
    }
#endif
}