export module Deduplicator;
import std;
import Data;
import Hasher;
import Mover;

struct FileInfo
{
    std::filesystem::path path;
    std::uintmax_t size;
    std::filesystem::file_time_type mtime;
};

// Выбирает "хранителя" из группы дубликатов
static const FileInfo& PickKeeper(const std::vector<FileInfo>& group, const std::filesystem::path& downloads) {
    const FileInfo* best = &group[0];

    for (size_t i = 1; i < group.size(); ++i)
    {
        const auto& curr = group[i];

        bool curr_in_cat = (curr.path.parent_path() != downloads);
        bool best_in_cat = (best->path.parent_path() != downloads);

        // Приоритет 1: файл уже в папке категории
        if (curr_in_cat && !best_in_cat)
        {
            best = &curr;
            continue;
        }
        else if (!curr_in_cat && best_in_cat)
            continue;

        // Приоритет 2: самый старый файл
        if (curr.mtime < best->mtime)
            best = &curr;
        // Приоритет 3: алфавитный порядок
        else if (curr.mtime == best->mtime && curr.path < best->path)
            best = &curr;
    }
    return *best;
}

// Преобразует хэш в строку для unordered_map
static std::string HashToString(const std::array<unsigned char, 32>& hash)
{
    return std::string(reinterpret_cast<const char*>(hash.data()), hash.size());
}

export int Deduplicate(const std::filesystem::path& downloads, const std::vector<std::filesystem::path>& files)
{
    if (files.size() < 2) return 0;

    std::vector<FileInfo> file_infos;
    std::error_code ec;

    for (const auto& p : files)
    {
        auto size = std::filesystem::file_size(p, ec);
        if (ec) continue;
        file_infos.push_back({ p, size, std::filesystem::last_write_time(p, ec) });
    }

    int duplicates_moved = 0;
    std::println("── Начинаем поиск дубликатов ({} файлов) ──", file_infos.size());

    // Шаг 1: Группировка по размеру (быстро)
    std::unordered_map<std::uintmax_t, std::vector<FileInfo>> size_groups;
    for (const auto& fi : file_infos) size_groups[fi.size].push_back(fi);

    for (auto& [size, group] : size_groups) {
        if (group.size() < 2) continue;

        // Шаг 2: Полный хэш (только для файлов с одинаковым размером)
        std::unordered_map<std::string, std::vector<FileInfo>> hash_groups;

        for (const auto& fi : group)
        {
            auto hash_opt = GetFullHash(fi.path);
            if (hash_opt)
                hash_groups[HashToString(*hash_opt)].push_back(fi);
        }

        // Шаг 3: Обработка найденных дубликатов
        for (auto& [f_hash, f_group] : hash_groups)
        {
            if (f_group.size() < 2) continue;

            const FileInfo& keeper = PickKeeper(f_group, downloads);

            for (const auto& fi : f_group)
            {
                if (fi.path == keeper.path) continue; // Хранителя не трогаем

                if (MoveFileToDuplicate(fi.path, downloads))
                    duplicates_moved++;
            }
        }
    }

    std::println("── Поиск дубликатов завершен. Перемещено копий: {} ──", duplicates_moved);
    return duplicates_moved;
}