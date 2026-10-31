#include "Data.h"
#include "Localization.h"
#include "Mover.h"
#include <iostream>
#include <system_error>

namespace fs = std::filesystem;

static fs::path GetUniquePath(const fs::path& dest_dir, const fs::path& original) {
    std::error_code ec;
    fs::path target = dest_dir / original.filename();
    if (!fs::exists(target, ec)) return target;

    std::string stem = original.stem().string();
    std::string ext = original.extension().string();
    for (int counter = 1; ; ++counter) {
        target = dest_dir / (stem + " (" + std::to_string(counter) + ")" + ext);
        if (!fs::exists(target, ec)) return target;
    }
}

static bool EnsureDirectory(const fs::path& dir) {
    std::error_code ec;
    if (fs::exists(dir, ec)) return true;
    fs::create_directories(dir, ec);
    if (ec) {
        std::cerr << Loc("[Ошибка] Не удалось создать папку ", "[Error] Failed to create folder ") << dir << ": " << ec.message() << std::endl;
        return false;
    }
    std::cout << Loc("[Создана папка] ", "[Created folder] ") << dir.filename() << std::endl;
    return true;
}

static bool SafeMove(const fs::path& from, const fs::path& to) {
    std::error_code ec;
    fs::rename(from, to, ec);

    if (ec) {
        fs::copy_file(from, to, fs::copy_options::overwrite_existing, ec);
        if (ec) return false;
        fs::remove(from, ec);
        if (ec) return false;
    }
    return true;
}

bool MoveFileToCategory(const fs::path& file, const fs::path& downloads) {
    std::string_view folder_name = FindFolderForExtension(file.extension().string());
    if (folder_name.empty()) folder_name = "+_Other";

    fs::path dest_dir = downloads / folder_name;

    std::error_code ec;
    if (fs::equivalent(file.parent_path(), dest_dir, ec) && !ec) return false;
    if (!EnsureDirectory(dest_dir)) return false;

    fs::path dest_path = GetUniquePath(dest_dir, file);
    if (SafeMove(file, dest_path)) {
        std::cout << Loc("  [Перемещен] ", "  [Moved] ") << file.filename() << " -> " << folder_name << std::endl;
        return true;
    }
    std::cerr << Loc("  [Ошибка] Не удалось переместить ", "  [Error] Failed to move ") << file.filename() << std::endl;
    return false;
}

bool MoveFileToDuplicate(const fs::path& file, const fs::path& downloads) {
    fs::path dest_dir = downloads / kDuplicateFolder;

    std::error_code ec;
    if (fs::equivalent(file.parent_path(), dest_dir, ec) && !ec) return false;
    if (!EnsureDirectory(dest_dir)) return false;

    fs::path dest_path = GetUniquePath(dest_dir, file);
    if (SafeMove(file, dest_path)) {
        std::cout << Loc("  [Дубликат] ", "  [Duplicate] ") << file.filename() << " -> +0_Duplicate" << std::endl;
        return true;
    }
    std::cerr << Loc("  [Ошибка] Не удалось переместить дубликат ", "  [Error] Failed to move duplicate ") << file.filename() << std::endl;
    return false;
}