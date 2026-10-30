module;
#include <windows.h>

export module Mover;
import std;
import Data;
import Console;

static std::filesystem::path GetUniquePath(const std::filesystem::path& dest_dir,
    const std::filesystem::path& original) {
    std::error_code ec;
    std::filesystem::path target = dest_dir / original.filename();
    if (!std::filesystem::exists(target, ec)) return target;

    const std::wstring stem = original.stem().wstring();
    const std::wstring ext = original.extension().wstring();
    for (int counter = 1; ; ++counter) {
        target = dest_dir / (stem + L" (" + std::to_wstring(counter) + L")" + ext);
        if (!std::filesystem::exists(target, ec)) return target;
    }
}

static bool EnsureDirectory(const std::filesystem::path& dir, std::wstring_view display_name) {
    std::error_code ec;
    if (std::filesystem::exists(dir, ec)) return true;

    std::filesystem::create_directories(dir, ec);
    const std::wstring name(display_name);
    if (ec) {
        std::println(std::cerr, "{}  [Ошибка] Создание папки {}: {}{}",
            RED, ToUtf8(name), ec.message(), RESET);
        return false;
    }
    std::println("{}  [Создана папка] {}{}", CYAN, ToUtf8(name), RESET);
    return true;
}

export bool MoveFileToCategory(const std::filesystem::path& file,
    const std::filesystem::path& downloads) {
    std::wstring_view folder_name = FindFolderForExtension(file.extension().wstring());
    if (folder_name.empty()) folder_name = L"+_Other";

    const std::filesystem::path dest_dir = downloads / folder_name;

    std::error_code ec;
    if (std::filesystem::equivalent(file.parent_path(), dest_dir, ec) && !ec)
        return false;
    if (!EnsureDirectory(dest_dir, folder_name))
        return false;

    const std::filesystem::path dest_path = GetUniquePath(dest_dir, file);
    if (MoveFileExW(file.c_str(), dest_path.c_str(), MOVEFILE_COPY_ALLOWED)) {
        std::println("{}  [Перемещен]{} {} -> {}", GREEN, RESET,
            ToUtf8(file.filename().wstring()), ToUtf8(folder_name));
        return true;
    }
    std::println(std::cerr, "{}  [Ошибка] Не удалось переместить {}{}",
        RED, ToUtf8(file.filename().wstring()), RESET);
    return false;
}

export bool MoveFileToDuplicate(const std::filesystem::path& file,
    const std::filesystem::path& downloads) {
    const std::filesystem::path dest_dir = downloads / kDuplicateFolder;

    std::error_code ec;
    if (std::filesystem::equivalent(file.parent_path(), dest_dir, ec) && !ec)
        return false;
    if (!EnsureDirectory(dest_dir, kDuplicateFolder))
        return false;

    const std::filesystem::path dest_path = GetUniquePath(dest_dir, file);
    if (MoveFileExW(file.c_str(), dest_path.c_str(), MOVEFILE_COPY_ALLOWED)) {
        std::println("{}  [Дубликат]{} {} -> {}", YELLOW, RESET,
            ToUtf8(file.filename().wstring()), ToUtf8(kDuplicateFolder));
        return true;
    }
    std::println(std::cerr, "{}  [Ошибка] Не удалось переместить дубликат {}{}",
        RED, ToUtf8(file.filename().wstring()), RESET);
    return false;
}