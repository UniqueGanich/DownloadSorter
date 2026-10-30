export module Sorter;

import std;
import CheckPath;
import Scanner;
import Mover;
import Deduplicator;

export bool sorter()
{
    auto downloads_opt = getDownloadsPath();
    if (!downloads_opt) return false;

    const std::filesystem::path downloads = *downloads_opt;
    std::println("Папка «Загрузки»: {}\n", downloads.string());

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

    // ИТОГИ
    std::println("\n=========================================");
    std::println("Всего перемещено по категориям: {}", moved_count);
    std::println("Всего найдено дубликатов: {}", dedupe_count);
    std::println("=========================================");

    return true;
}