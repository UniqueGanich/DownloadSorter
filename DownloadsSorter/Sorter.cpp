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

export bool sorter()
{
    std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

    auto downloads_opt = getDownloadsPath();
    if (!downloads_opt) return false;

    const std::filesystem::path downloads = *downloads_opt;
    std::println("{}Папка «Загрузки»:{} {}\n", BOLD, RESET, downloads.string());

    std::set<std::filesystem::path> existing_folders;
    std::error_code ec;

    for (auto folder_name : kCategoryFolders)
    {
        std::filesystem::path p = downloads / folder_name;
        if (std::filesystem::exists(p, ec)) existing_folders.insert(p);
    }
    std::filesystem::path dup_dir = downloads / kDuplicateFolder;
    if (std::filesystem::exists(dup_dir, ec)) existing_folders.insert(dup_dir);

    // ПРОХОД 1: Сортировка
    auto scan_res = ScanDownloads(downloads);
    int moved_count = 0;

    if (!scan_res.files.empty())
    {
        std::println("{}── Начинаем сортировку ({} файлов) ──{}", BOLD, scan_res.files.size(), RESET);
        for (const std::filesystem::path& file : scan_res.files)
            if (MoveFileToCategory(file, downloads)) moved_count++;
    }
    else std::println("Нет файлов для сортировки.");

    // ПРОХОД 2: Дедупликация
    auto scan_res_dedupe = ScanDownloads(downloads);
    std::println("");
    int dedupe_count = Deduplicate(downloads, scan_res_dedupe.files);

    // Уборка пустых папок
    int removed_folders = 0;
    for (auto folder_name : kCategoryFolders) {
        std::filesystem::path p = downloads / folder_name;
        if (existing_folders.find(p) == existing_folders.end() && std::filesystem::exists(p, ec) && std::filesystem::is_empty(p, ec))
        {
            std::filesystem::remove(p, ec);
            if (!ec)
            {
                std::println("{}  [Удалена пустая папка] {}{}", CYAN, std::filesystem::path(folder_name).string(), RESET);
                removed_folders++;
            }
        }
    }
    if (existing_folders.find(dup_dir) == existing_folders.end() && std::filesystem::exists(dup_dir, ec) && std::filesystem::is_empty(dup_dir, ec))
    {
        std::filesystem::remove(dup_dir, ec);
        if (!ec)
        {
            std::println("{}  [Удалена пустая папка] +0_Duplicate{}", CYAN, RESET);
            removed_folders++;
        }
    }

    // ИТОГИ
    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();

    auto diff = end_time - start_time;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
    long long duration = ms.count();

    std::println("\n{}{}============ СТАТИСТИКА ============{}", BOLD, CYAN, RESET);
    std::println("{}Время работы:{}      {} мс", BOLD, RESET, duration);
    std::println("{}Перемещено файлов:{} {}", BOLD, RESET, moved_count);
    std::println("{}Найдено дубликатов:{} {}", BOLD, RESET, dedupe_count);
    std::println("{}Удалено папок:{}     {}", BOLD, RESET, removed_folders);

    if (scan_res.skipped_temp > 0 || scan_res.skipped_fresh > 0 || scan_res.skipped_locked > 0)
    {
        std::println("\n{}Пропущено (осталось в покое):{}", BOLD, RESET);
        if (scan_res.skipped_temp > 0)   std::println("  {}• Незавершенные загрузки:{} {}", YELLOW, RESET, scan_res.skipped_temp);
        if (scan_res.skipped_fresh > 0)  std::println("  {}• Скачаны менее 24ч назад:{} {}", YELLOW, RESET, scan_res.skipped_fresh);
        if (scan_res.skipped_locked > 0) std::println("  {}• Заняты другой программой:{} {}", RED, RESET, scan_res.skipped_locked);
    }
    std::println("{}{}====================================={}\n", BOLD, CYAN, RESET);

    return true;
}