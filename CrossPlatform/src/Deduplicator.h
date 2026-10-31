#pragma once
#include <filesystem>
#include <vector>

int Deduplicate(const std::filesystem::path& downloads, const std::vector<std::filesystem::path>& files);