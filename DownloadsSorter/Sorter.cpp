module;
#include <chrono>

export module Sorter;

import std;
import Scanner;
import Mover;
import Deduplicator;
import Data;
import Console;
import Localization;

static int RemoveEmptyFolders(const std::filesystem::path& downloads) {
    int removed = 0;
    std::error_code ec;

    auto try_remove = [&](const std::filesystem::path& p, std::wstring_view name) {
        ec.clear();
        if (!std::filesystem::exists(p, ec) || !std::filesystem::is_empty(p, ec))
            return;
        std::filesystem::remove(p, ec);
        if (!ec) {
            std::println("{}  [{}] {}{}", CYAN, Loc("Удалена пустая папка", "Deleted empty folder"), ToUtf8(name), RESET);
            ++removed;
        }
        else {
            std::println(std::cerr, "{}  [{}] {} {}: {}{}",
                RED, Loc("Ошибка", "Error"), Loc("Не удалось удалить", "Failed to delete"), ToUtf8(name), ec.message(), RESET);
        }
        };

    for (auto folder_name : kCategoryFolders)
        try_remove(downloads / folder_name, folder_name);
    try_remove(downloads / kDuplicateFolder, kDuplicateFolder);

    return removed;
}

export bool sorter(const std::filesystem::path& downloads) {
    auto start_time = std::chrono::steady_clock::now();

    std::println("{} {}:{} {}\n", BOLD, Loc("Папка сортировки:", "Sorting folder:"), RESET, ToUtf8(downloads.wstring()));

    // Проход 1: сортировка
    auto scan_res = ScanDownloads(downloads);
    int moved_count = 0;
    if (!scan_res.files.empty()) {
        std::println("{}── {} ({} {}) ──{}", BOLD, Loc("Начинаем сортировку", "Starting sorting"), scan_res.files.size(), Loc("файлов", "files"), RESET);
        for (const auto& file : scan_res.files)
            if (MoveFileToCategory(file, downloads)) ++moved_count;
    }
    else {
        std::println("{}", Loc("Нет файлов для сортировки.", "No files to sort."));
    }

    // Проход 2: дедупликация
    auto scan_res_dedupe = ScanDownloads(downloads);
    std::println("");
    int dedupe_count = Deduplicate(downloads, scan_res_dedupe.files);

    // Проход 3: уборка пустых папок
    int removed_folders = RemoveEmptyFolders(downloads);

    // Итоги
    auto diff = std::chrono::steady_clock::now() - start_time;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    std::println("\n{}{}============ {} ============{}", BOLD, CYAN, Loc("СТАТИСТИКА", "STATISTICS"), RESET);
    std::println("{} {}:{}      {} {}", BOLD, Loc("Время работы", "Execution time"), RESET, duration, Loc("мс", "ms"));
    std::println("{} {}:{} {}", BOLD, Loc("Перемещено файлов", "Files moved"), RESET, moved_count);
    std::println("{} {}:{} {}", BOLD, Loc("Найдено дубликатов", "Duplicates found"), RESET, dedupe_count);
    std::println("{} {}:{}     {}", BOLD, Loc("Удалено папок", "Folders deleted"), RESET, removed_folders);

    if (scan_res.skipped_temp || scan_res.skipped_locked) {
        std::println("\n{}{}:{}", BOLD, Loc("Пропущено (осталось в покое)", "Skipped (left untouched)"), RESET);
        if (scan_res.skipped_temp)   std::println("  {}• {}:{} {}", YELLOW, Loc("Временные файлы (идет загрузка)", "Temporary files (downloading)"), RESET, scan_res.skipped_temp);
        if (scan_res.skipped_locked) std::println("  {}• {}:{} {}", YELLOW, Loc("Заняты или только что изменены", "Locked or recently modified"), RESET, scan_res.skipped_locked);
    }
    std::println("{}{}====================================={}\n", BOLD, CYAN, RESET);

    return true;
}