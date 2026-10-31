#include "Data.h"
#include "Deduplicator.h"
#include "Localization.h"
#include "Mover.h"
#include "Scanner.h"
#include "Sorter.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

static bool IsEffectivelyEmpty(const fs::path& p) {
    std::error_code ec;
    if (!fs::is_directory(p, ec) || ec) return false;

    for (const auto& entry : fs::directory_iterator(p, ec)) {
        if (ec) return false;
        std::string filename = entry.path().filename().string();

        if (filename == "desktop.ini" || filename == "thumbs.db" || filename == ".DS_Store" || filename.starts_with(".")) {
            continue;
        }

#ifdef _WIN32
        DWORD attrs = GetFileAttributesW(entry.path().c_str());
        if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
            continue;
        }
#endif
        return false;
    }
    return true;
}

static int RemoveEmptyFolders(const fs::path& downloads) {
    int removed = 0;
    std::error_code ec;
    auto try_remove = [&](const fs::path& p) {
        ec.clear();
        if (fs::exists(p, ec) && IsEffectivelyEmpty(p)) {
            fs::remove_all(p, ec);
            if (!ec) {
                std::cout << Loc("  [Удалена пустая папка] ", "  [Deleted empty folder] ") << p.filename() << std::endl;
                removed++;
            }
        }
        };
    for (auto folder : kCategoryFolders) try_remove(downloads / folder);
    try_remove(downloads / kDuplicateFolder);
    return removed;
}

bool SortDownloads(const fs::path& downloads) {
    std::cout << Loc("Папка сортировки: ", "Sorting folder: ") << downloads << "\n\n";

    auto scan_res = ScanDownloads(downloads);
    int moved = 0;

    if (!scan_res.files.empty()) {
        std::cout << Loc("-- Начинаем сортировку (", "-- Starting sort (") << scan_res.files.size() << Loc(" файлов) --", " files) --") << std::endl;
        for (const auto& file : scan_res.files) {
            if (MoveFileToCategory(file, downloads)) moved++;
        }
    }
    else {
        std::cout << Loc("Нет файлов для сортировки.", "No files to sort.") << std::endl;
    }

    auto scan_dedupe = ScanDownloads(downloads);
    int dedupe = Deduplicate(downloads, scan_dedupe.files);
    int removed = RemoveEmptyFolders(downloads);

    std::cout << "\n" << Loc("============ СТАТИСТИКА ============", "============ STATISTICS ============") << std::endl;
    std::cout << Loc("Перемещено файлов:  ", "Files moved:      ") << moved << std::endl;
    std::cout << Loc("Найдено дубликатов: ", "Duplicates found: ") << dedupe << std::endl;
    std::cout << Loc("Удалено папок:      ", "Folders deleted:  ") << removed << std::endl;

    if (scan_res.skipped_temp || scan_res.skipped_locked) {
        std::cout << Loc("Пропущено: ", "Skipped: ");
        if (scan_res.skipped_temp) std::cout << Loc("[Временные: ", "[Temp: ") << scan_res.skipped_temp << "] ";
        if (scan_res.skipped_locked) std::cout << Loc("[Занятые: ", "[Locked: ") << scan_res.skipped_locked << "] ";
        std::cout << std::endl;
    }
    std::cout << "====================================\n" << std::endl;
    return true;
}