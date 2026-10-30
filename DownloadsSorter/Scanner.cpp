export module Scanner;
import std;
import Data;
import FileUtils;
import Console;

export struct ScanResult {
    std::vector<std::filesystem::path> files;
    int skipped_temp = 0;
    int skipped_fresh = 0;
    int skipped_locked = 0;
};

template<typename Iterator>
static void ProcessEntries(Iterator it, Iterator end, ScanResult& result, std::wstring_view context) {
    auto handle = [&](const std::filesystem::directory_entry& entry) {
        if (!entry.is_regular_file() || entry.is_symlink()) return;
        switch (CheckFileStatus(entry.path())) {
        case FileStatus::Eligible:  result.files.push_back(entry.path()); break;
        case FileStatus::Temporary: result.skipped_temp++; break;
        case FileStatus::TooFresh:  result.skipped_fresh++; break;
        case FileStatus::Locked:    result.skipped_locked++; break;
        default: break;
        }
        };

    std::error_code ec;
    while (it != end) {
        handle(*it);
        it.increment(ec);
        if (ec) {
            std::println(std::cerr, "{}  [Ошибка] Обход {}: {}{}",
                RED, ToUtf8(context), ec.message(), RESET);
            break;
        }
    }
}

export ScanResult ScanDownloads(const std::filesystem::path& downloads) {
    ScanResult result;
    std::error_code ec;
    const auto opts = std::filesystem::directory_options::skip_permission_denied;

    ProcessEntries(
        std::filesystem::directory_iterator(downloads, opts, ec),
        std::filesystem::directory_iterator(),
        result, L"корня");

    for (auto folder_name : kCategoryFolders) {
        const std::filesystem::path folder_path = downloads / folder_name;
        ec.clear();
        if (!std::filesystem::is_directory(folder_path, ec) || ec) continue;

        ProcessEntries(
            std::filesystem::recursive_directory_iterator(folder_path, opts, ec),
            std::filesystem::recursive_directory_iterator(),
            result, folder_name);
    }
    return result;
}