export module Scanner;
import std;
import Data;
import FileUtils;
import Console;

export struct ScanResult
{
    std::vector<std::filesystem::path> files;
    int skipped_temp = 0;
    int skipped_fresh = 0;
    int skipped_locked = 0;
};

export ScanResult ScanDownloads(const std::filesystem::path& downloads)
{
    ScanResult result;
    std::error_code ec;
    auto dir_options = std::filesystem::directory_options::skip_permission_denied;

    auto process_file = [&](const std::filesystem::path& p)
        {
            FileStatus status = CheckFileStatus(p);
            switch (status)
            {
            case FileStatus::Eligible:  result.files.push_back(p); break;
            case FileStatus::Temporary: result.skipped_temp++; break;
            case FileStatus::TooFresh:  result.skipped_fresh++; break;
            case FileStatus::Locked:    result.skipped_locked++; break;
            case FileStatus::Ignored:   break;
            default: break;
            }
        };

    // 1. Корень
    auto it_root = std::filesystem::directory_iterator(downloads, dir_options, ec);
    auto end_it = std::filesystem::directory_iterator();

    while (it_root != end_it)
    {
        const auto& entry = *it_root;
        if (entry.is_regular_file() && !entry.is_symlink())
            process_file(entry.path());

        it_root.increment(ec);
        if (ec) {
            std::println(std::cerr, "{}  [Ошибка] Прерван обход корня из-за сбоя ФС{}", RED, RESET);
            break;
        }
    }

    // 2. Наши папки
    for (auto folder_name : kCategoryFolders)
    {
        std::filesystem::path folder_path = downloads / folder_name;

        ec.clear();
        if (!std::filesystem::is_directory(folder_path, ec) || ec) continue;

        auto it_rec = std::filesystem::recursive_directory_iterator(folder_path, dir_options, ec);
        auto end_rec = std::filesystem::recursive_directory_iterator();

        while (it_rec != end_rec)
        {
            const auto& entry = *it_rec;
            if (entry.is_regular_file() && !entry.is_symlink())
                process_file(entry.path());

            it_rec.increment(ec);
            if (ec) {
                std::println(std::cerr, "{}  [Ошибка] Прерван обход папки {}{}", RED, ToUtf8(std::wstring(folder_name)), RESET);
                break;
            }
        }
    }
    return result;
}