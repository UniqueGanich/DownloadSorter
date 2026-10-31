#pragma once
#include "Config.h"
#include <filesystem>

void ShowMainMenu(AppConfig& cfg, const std::filesystem::path& currentTarget);
void ShowAutoRunMenu(AppConfig& cfg);
void ShowFolderMenu(AppConfig& cfg);