#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <array>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

std::optional<std::array<unsigned char, 32>> GetQuickHash(const std::filesystem::path& file);
std::optional<std::array<unsigned char, 32>> GetFullHash(const std::filesystem::path& file);