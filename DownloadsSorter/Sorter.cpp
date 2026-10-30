export module Sorter;

import std;
import CheckPath;
import Scanner;
import Mover;
import Deduplicator;
import Data;

export bool sorter()
{
    auto downloads_opt = getDownloadsPath();
    if (!downloads_opt) return false;

    const std::filesystem::path downloads = *downloads_opt;
    std::println("Папка «Загрузки»: {}\n", downloads.string());

    // Запоминаем, какие папки уже были до запуска
    std::set<std::filesystem::path> existing_folders;
    std::error_code ec;

    for (auto folder_name : kCategoryFolders)
    {
        std::filesystem::path p = downloads / folder_name;
        if (std::filesystem::exists(p, ec)) existing_folders.insert(p);
    }
    std::filesystem::path dup_dir = downloads / kDuplicateFolder;
    if (std::filesystem::exists(dup_dir, ec)) existing_folders.insert(dup_dir);

    // ПРОХОД 1: Сортировка по категориям
    auto files_to_process = ScanDownloads(downloads);

    int moved_count = 0;
    if (!files_to_process.empty())
    {
        std::println("── Начинаем сортировку ({} файлов) ──", files_to_process.size());
        for (const auto& file : files_to_process)
            if (MoveFileToCategory(file, downloads)) moved_count++;
    }
    else std::println("Нет файлов для сортировки.");

    // ПРОХОД 2: Дедупликация
    auto files_for_dedupe = ScanDownloads(downloads);
    int dedupe_count = Deduplicate(downloads, files_for_dedupe);

    // Уборка пустых папок, созданных во время этого запуска
    int removed_folders = 0;

    // Проходимся по обычным категориям
    for (auto folder_name : kCategoryFolders)
    {
        std::filesystem::path p = downloads / folder_name;
        if (existing_folders.find(p) == existing_folders.end() &&
            std::filesystem::exists(p, ec) &&
            std::filesystem::is_empty(p, ec))
        {
            std::filesystem::remove(p, ec);
            if (!ec)
            {
                std::println("  [Удалена пустая папка] {}", std::string(folder_name.begin(), folder_name.end()));
                removed_folders++;
            }
        }
    }

    // Отдельно проходимся по папке дубликатов
    if (existing_folders.find(dup_dir) == existing_folders.end() &&
        std::filesystem::exists(dup_dir, ec) &&
        std::filesystem::is_empty(dup_dir, ec))
    {
        std::filesystem::remove(dup_dir, ec);
        if (!ec)
        {
            std::println("  [Удалена пустая папка] {}", std::string(kDuplicateFolder.begin(), kDuplicateFolder.end()));
            removed_folders++;
        }
    }

    // ИТОГИ
    std::println("\n=========================================");
    std::println("Всего перемещено по категориям: {}", moved_count);
    std::println("Всего найдено дубликатов: {}", dedupe_count);
    std::println("Удалено пустых папок: {}", removed_folders);
    std::println("=========================================");

    return true;
}