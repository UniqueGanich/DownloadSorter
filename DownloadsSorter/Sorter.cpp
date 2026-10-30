module;
#include <chrono>

export module Sorter;

import std;
import CheckPath;
import Scanner;
import Mover;
import Deduplicator;
import Data;
import Console;

static int RemoveEmptyFolders(const std::filesystem::path& downloads) {
    int removed = 0;
    std::error_code ec;

    auto try_remove = [&](const std::filesystem::path& p, std::wstring_view name) {
        ec.clear();
        if (!std::filesystem::exists(p, ec) || !std::filesystem::is_empty(p, ec))
            return;
        std::filesystem::remove(p, ec);
        if (!ec) {
            std::println("{}  [Удалена пустая папка] {}{}", CYAN, ToUtf8(name), RESET);
            ++removed;
        }
        else {
            std::println(std::cerr, "{}  [Ошибка] Не удалось удалить {}: {}{}",
                RED, ToUtf8(name), ec.message(), RESET);
        }
        };

    for (auto folder_name : kCategoryFolders)
        try_remove(downloads / folder_name, folder_name);
    try_remove(downloads / kDuplicateFolder, kDuplicateFolder);

    return removed;
}

export bool sorter() {
    auto start_time = std::chrono::steady_clock::now();

    auto downloads_opt = getDownloadsPath();
    if (!downloads_opt) return false;
    const std::filesystem::path downloads = *downloads_opt;
    std::println("{}Папка «Загрузки»:{} {}\n", BOLD, RESET, ToUtf8(downloads.wstring()));

    // Проход 1: сортировка
    auto scan_res = ScanDownloads(downloads);
    int moved_count = 0;
    if (!scan_res.files.empty()) {
        std::println("{}── Начинаем сортировку ({} файлов) ──{}", BOLD, scan_res.files.size(), RESET);
        for (const auto& file : scan_res.files)
            if (MoveFileToCategory(file, downloads)) ++moved_count;
    }
    else {
        std::println("Нет файлов для сортировки.");
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

    std::println("\n{}{}============ СТАТИСТИКА ============{}", BOLD, CYAN, RESET);
    std::println("{}Время работы:{}      {} мс", BOLD, RESET, duration);
    std::println("{}Перемещено файлов:{} {}", BOLD, RESET, moved_count);
    std::println("{}Найдено дубликатов:{} {}", BOLD, RESET, dedupe_count);
    std::println("{}Удалено папок:{}     {}", BOLD, RESET, removed_folders);

    if (scan_res.skipped_temp || scan_res.skipped_locked) {
        std::println("\n{}Пропущено (осталось в покое):{}", BOLD, RESET);
        if (scan_res.skipped_temp)   std::println("  {}• Временные файлы (идет загрузка):{} {}", YELLOW, RESET, scan_res.skipped_temp);
        if (scan_res.skipped_locked) std::println("  {}• Заняты или только что изменены:{} {}", YELLOW, RESET, scan_res.skipped_locked);
    }
    std::println("{}{}====================================={}\n", BOLD, CYAN, RESET);

    return true;
}