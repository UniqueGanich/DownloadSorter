export module Scanner;
import std;
import Data;
import FileUtils;

export std::vector<std::filesystem::path> ScanDownloads(const std::filesystem::path& downloads)
{
    std::vector<std::filesystem::path> files_to_process;
    std::error_code ec;
    auto dir_options = std::filesystem::directory_options::skip_permission_denied;

    for (const auto& entry : std::filesystem::directory_iterator(downloads, dir_options, ec))
    {
        if (ec) { ec.clear(); continue; }
        if (!entry.is_regular_file() || entry.is_symlink()) continue;

        if (IsFileEligible(entry.path()))
            files_to_process.push_back(entry.path());
    }

    for (auto folder_name : kCategoryFolders)
    {
        std::filesystem::path folder_path = downloads / folder_name;
        if (!std::filesystem::exists(folder_path) || !std::filesystem::is_directory(folder_path)) continue;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(folder_path, dir_options, ec))
        {
            if (ec) { ec.clear(); continue; }
            if (!entry.is_regular_file() || entry.is_symlink()) continue;

            if (IsFileEligible(entry.path()))
                files_to_process.push_back(entry.path());
        }
    }
    return files_to_process;
}