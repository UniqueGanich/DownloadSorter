#pragma once
#include <filesystem>
#include <string>

struct AppConfig {
    std::filesystem::path targetFolder;
    int autoRunMode = 0;
    int customTimeHours = 1;
};

std::filesystem::path GetConfigPath();
void SaveConfig(const AppConfig& cfg);
AppConfig LoadConfig();
void ApplyAutoRunSettings(const AppConfig& cfg);