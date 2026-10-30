module;
#include <windows.h>

export module Mover;
import std;
import Data;
import Console;

export std::filesystem::path GetUniquePath(const std::filesystem::path& dest_dir, const std::filesystem::path& original_file) {
    std::filesystem::path target = dest_dir / original_file.filename();
    std::error_code ec;

    if (!std::filesystem::exists(target, ec)) return target;

    std::wstring stem = original_file.stem().wstring();
    std::wstring ext = original_file.extension().wstring();
    int counter = 1;
    while (true)
    {
        std::wstring new_name = stem + L" (" + std::to_wstring(counter) + L")" + ext;
        target = dest_dir / new_name;
        if (!std::filesystem::exists(target, ec)) return target;
        counter++;
    }
}

export bool MoveFileToCategory(const std::filesystem::path& file, const std::filesystem::path& downloads)
{
    std::wstring ext = file.extension().wstring();
    std::wstring_view folder_name = FindFolderForExtension(ext);
    if (folder_name.empty()) folder_name = L"+_Other";

    std::filesystem::path dest_dir = downloads / folder_name;

    std::error_code ec;
    if (std::filesystem::equivalent(file.parent_path(), dest_dir, ec) && !ec) {
        return false;
    }

    if (!std::filesystem::exists(dest_dir, ec))
    {
        std::filesystem::create_directories(dest_dir, ec);
        if (ec)
        {
            std::println(std::cerr, "{}  [Ошибка] Создание папки {}: {}{}", RED, ToUtf8(std::wstring(folder_name)), ec.message(), RESET);
            return false;
        }
        std::println("{}  [Создана папка] {}{}", CYAN, ToUtf8(std::wstring(folder_name)), RESET);
    }

    std::filesystem::path dest_path = GetUniquePath(dest_dir, file);

    if (MoveFileExW(file.c_str(), dest_path.c_str(), MOVEFILE_COPY_ALLOWED))
    {
        std::println("{}  [Перемещен]{} {} -> {}", GREEN, RESET, ToUtf8(file.filename().wstring()), ToUtf8(std::wstring(folder_name)));
        return true;
    }
    else
    {
        std::println(std::cerr, "{}  [Ошибка] Не удалось переместить {}{}", RED, ToUtf8(file.filename().wstring()), RESET);
        return false;
    }
}

export bool MoveFileToDuplicate(const std::filesystem::path& file, const std::filesystem::path& downloads)
{
    std::filesystem::path dest_dir = downloads / kDuplicateFolder;

    std::error_code ec;
    if (std::filesystem::equivalent(file.parent_path(), dest_dir, ec) && !ec) {
        return false;
    }

    if (!std::filesystem::exists(dest_dir, ec))
    {
        std::filesystem::create_directories(dest_dir, ec);
        if (ec)
        {
            std::println(std::cerr, "{}  [Ошибка] Создание папки дубликатов: {}{}", RED, ec.message(), RESET);
            return false;
        }
        std::println("{}  [Создана папка] {}{}", CYAN, ToUtf8(std::wstring(kDuplicateFolder)), RESET);
    }

    std::filesystem::path dest_path = GetUniquePath(dest_dir, file);
    if (MoveFileExW(file.c_str(), dest_path.c_str(), MOVEFILE_COPY_ALLOWED))
    {
        std::println("{}  [Дубликат]{} {} -> {}", YELLOW, RESET, ToUtf8(file.filename().wstring()), ToUtf8(std::wstring(kDuplicateFolder)));
        return true;
    }
    else
    {
        std::println(std::cerr, "{}  [Ошибка] Не удалось переместить дубликат {}{}", RED, ToUtf8(file.filename().wstring()), RESET);
        return false;
    }
}