export module Scanner;
import std;
import Data;
import FileUtils;

export std::vector<std::filesystem::path> ScanDownloads(const std::filesystem::path& downloads)
{
    std::vector<std::filesystem::path> files_to_process;
    std::error_code ec;
    auto dir_options = std::filesystem::directory_options::skip_permission_denied;

    auto it_root = std::filesystem::directory_iterator(downloads, dir_options, ec);
    auto end_it = std::filesystem::directory_iterator();

    while (it_root != end_it) {
        if (ec) { ec.clear(); it_root.increment(ec); continue; }

        const auto& entry = *it_root;
        if (entry.is_regular_file() && !entry.is_symlink()) {
            if (IsFileEligible(entry.path()))
                files_to_process.push_back(entry.path());
        }
        it_root.increment(ec);
    }

    for (auto folder_name : kCategoryFolders)
    {
        std::filesystem::path folder_path = downloads / folder_name;
        if (!std::filesystem::exists(folder_path) || !std::filesystem::is_directory(folder_path)) continue;

        auto it_rec = std::filesystem::recursive_directory_iterator(folder_path, dir_options, ec);
        auto end_rec = std::filesystem::recursive_directory_iterator();

        while (it_rec != end_rec) {
            if (ec) { ec.clear(); it_rec.increment(ec); continue; }

            const auto& entry = *it_rec;
            if (entry.is_regular_file() && !entry.is_symlink()) {
                if (IsFileEligible(entry.path()))
                    files_to_process.push_back(entry.path());
            }
            it_rec.increment(ec);
        }
    }
    return files_to_process;
}