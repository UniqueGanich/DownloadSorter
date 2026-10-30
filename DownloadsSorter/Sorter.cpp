export module Sorter;
import std;
import CheckPath;
import Scanner;
import Mover;

export bool sorter()
{
    auto downloads_opt = getDownloadsPath();
    if (!downloads_opt) return false;

    const std::filesystem::path downloads = *downloads_opt;
    std::println("Папка «Загрузки»: {}\n", downloads.string());

    auto files_to_process = ScanDownloads(downloads);

    if (files_to_process.empty()) {
        std::println("Нет файлов для сортировки.");
        return true;
    }

    std::println("── Начинаем сортировку ({} файлов) ──", files_to_process.size());

    int moved_count = 0;

    for (const auto& file : files_to_process) {
        if (MoveFileToCategory(file, downloads)) {
            moved_count++;
        }
    }

    std::println("\n=========================================");
    std::println("Всего перемещено файлов: {}", moved_count);
    std::println("=========================================");

    return true;
}