export module Sorter;
import std;
import CheckPath;
import Scanner;

export bool sorter()
{
    auto downloads_opt = getDownloadsPath();
    if (!downloads_opt) return false;

    const std::filesystem::path downloads = *downloads_opt;
    std::println("Папка «Загрузки»: {}\n", downloads.string());

    auto files_to_process = ScanDownloads(downloads);
}