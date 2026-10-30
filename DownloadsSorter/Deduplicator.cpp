export module Deduplicator;
import std;
import Data;
import Hasher;
import Mover;

struct FileInfo {
    std::filesystem::path path;
    std::uintmax_t size = 0; // <-- ИСПРАВЛЕНО: инициализация по умолчанию
    std::filesystem::file_time_type mtime{}; // <-- ИСПРАВЛЕНО: инициализация по умолчанию
};

static const FileInfo& PickKeeper(const std::vector<FileInfo>& group, const std::filesystem::path& downloads) {
    const FileInfo* best = &group[0];
    for (size_t i = 1; i < group.size(); ++i) {
        const auto& curr = group[i];
        const bool curr_in_cat = curr.path.parent_path() != downloads;
        const bool best_in_cat = best->path.parent_path() != downloads;

        if (curr_in_cat != best_in_cat) {
            if (curr_in_cat) best = &curr;
            continue;
        }
        if (curr.mtime < best->mtime || (curr.mtime == best->mtime && curr.path < best->path))
            best = &curr;
    }
    return *best;
}

static std::string HashToString(const std::array<unsigned char, 32>& hash) {
    return std::string(reinterpret_cast<const char*>(hash.data()), hash.size());
}

export int Deduplicate(const std::filesystem::path& downloads, const std::vector<std::filesystem::path>& files) {
    if (files.size() < 2) return 0;

    std::vector<FileInfo> file_infos;
    std::error_code ec;
    for (const auto& p : files) {
        auto size = std::filesystem::file_size(p, ec);
        if (ec) continue;
        auto mtime = std::filesystem::last_write_time(p, ec);
        if (ec) continue;
        file_infos.push_back({ p, size, mtime });
    }

    int duplicates_moved = 0;
    std::println("── Начинаем поиск дубликатов ({} файлов) ──", file_infos.size());

    std::unordered_map<std::uintmax_t, std::vector<FileInfo>> size_groups;
    for (const auto& fi : file_infos) size_groups[fi.size].push_back(fi);

    unsigned int max_threads = std::thread::hardware_concurrency();
    if (max_threads == 0) max_threads = 4;

    for (auto& [_, group] : size_groups) {
        if (group.size() < 2) continue;

        std::unordered_map<std::string, std::vector<FileInfo>> quick_groups;

        // ЭТАП 1: Быстрый хэш (4 КБ) - запускаем пачками
        for (size_t i = 0; i < group.size(); i += max_threads) {
            std::vector<std::future<std::pair<FileInfo, std::optional<std::array<unsigned char, 32>>>>> futures;
            size_t end = (std::min)(i + max_threads, group.size());

            for (size_t j = i; j < end; ++j) {
                futures.push_back(std::async(std::launch::async, [fi = group[j]]() {
                    return std::make_pair(fi, GetQuickHash(fi.path));
                    }));
            }

            for (auto& fut : futures) {
                auto [fi, hash_opt] = fut.get();
                if (hash_opt) quick_groups[HashToString(*hash_opt)].push_back(fi);
            }
        }

        // ЭТАП 2: Полный хэш
        for (auto& [q_hash, q_group] : quick_groups) {
            if (q_group.size() < 2) continue;

            std::unordered_map<std::string, std::vector<FileInfo>> full_groups;

            for (size_t i = 0; i < q_group.size(); i += max_threads) {
                std::vector<std::future<std::pair<FileInfo, std::optional<std::array<unsigned char, 32>>>>> futures;
                size_t end = (std::min)(i + max_threads, q_group.size());

                for (size_t j = i; j < end; ++j) {
                    futures.push_back(std::async(std::launch::async, [fi = q_group[j]]() {
                        return std::make_pair(fi, GetFullHash(fi.path));
                        }));
                }

                for (auto& fut : futures) {
                    auto [fi, hash_opt] = fut.get();
                    if (hash_opt) full_groups[HashToString(*hash_opt)].push_back(fi);
                }
            }

            // ЭТАП 3: Перемещение
            for (auto& [f_hash, f_group] : full_groups) {
                if (f_group.size() < 2) continue;
                const FileInfo& keeper = PickKeeper(f_group, downloads);
                for (const auto& fi : f_group) {
                    if (fi.path != keeper.path && MoveFileToDuplicate(fi.path, downloads))
                        duplicates_moved++;
                }
            }
        }
    }

    std::println("── Поиск дубликатов завершен. Перемещено копий: {} ──", duplicates_moved);
    return duplicates_moved;
}