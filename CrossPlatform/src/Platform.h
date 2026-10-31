#pragma once
#include <filesystem>
#include <string>

std::filesystem::path GetDownloadPath();
bool IsFileLocked(const std::filesystem::path& file);
std::string ToUtf8(const std::wstring& wstr);
char GetChar();