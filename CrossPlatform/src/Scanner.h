#pragma once
#include <filesystem>
#include <vector>

struct ScanResult {
    std::vector<std::filesystem::path> files;
    int skipped_temp = 0;
    int skipped_locked = 0;
};

ScanResult ScanDownloads(const std::filesystem::path& downloads);