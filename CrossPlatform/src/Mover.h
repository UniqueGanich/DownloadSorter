#pragma once
#include <filesystem>

bool MoveFileToCategory(const std::filesystem::path& file, const std::filesystem::path& downloads);
bool MoveFileToDuplicate(const std::filesystem::path& file, const std::filesystem::path& downloads);