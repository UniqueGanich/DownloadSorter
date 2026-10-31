#pragma once
#include <array>
#include <string>
#include <string_view>

extern const std::string kDuplicateFolder;
extern const std::array<std::string_view, 10> kCategoryFolders;

std::string_view FindFolderForExtension(std::string_view ext);
bool IsTemporaryExtension(std::string_view ext);