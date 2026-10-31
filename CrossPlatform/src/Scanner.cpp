#include "Data.h"
#include "Platform.h"
#include "Scanner.h"
#include <chrono>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

static void ProcessFile(const fs::path& p, ScanResult& result) {
    std::error_code ec;
    if (!fs::is_regular_file(p, ec) || ec) return;

    std::string filename = p.filename().string();
#ifdef _WIN32
    if (filename == "desktop.ini" || filename == "thumbs.db") return;

    DWORD attrs = GetFileAttributesW(p.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) return;
#else
    if (filename.starts_with(".")) return;
#endif

    if (IsTemporaryExtension(p.extension().string())) {
        result.skipped_temp++;
        return;
    }

    auto ftime = fs::last_write_time(p, ec);
    if (!ec) {
        auto age = fs::file_time_type::clock::now() - ftime;
        if (age <= std::chrono::seconds(5)) {
            result.skipped_locked++;
            return;
        }
    }

#ifndef _WIN32
    if (IsFileLocked(p)) {
        result.skipped_locked++;
        return;
    }
#endif

    result.files.push_back(p);
}

ScanResult ScanDownloads(const fs::path& downloads) {
    ScanResult result;
    std::error_code ec;

    for (const auto& entry : fs::directory_iterator(downloads, fs::directory_options::skip_permission_denied, ec)) {
        if (ec) break;
        ProcessFile(entry.path(), result);
    }

    for (auto folder_name : kCategoryFolders) {
        fs::path folder_path = downloads / folder_name;
        ec.clear();
        if (!fs::is_directory(folder_path, ec) || ec) continue;

        for (const auto& entry : fs::recursive_directory_iterator(folder_path, fs::directory_options::skip_permission_denied, ec)) {
            if (ec) break;
            ProcessFile(entry.path(), result);
        }
    }
    return result;
}