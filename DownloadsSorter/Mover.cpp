module;
#include <windows.h>

export module Mover;
import std;
import Data;

// Генерирует уникальное имя файла, если в папке уже есть такой файл
export std::filesystem::path GetUniquePath(const std::filesystem::path& dest_dir, const std::filesystem::path& original_file) {
    std::filesystem::path target = dest_dir / original_file.filename();

    if (!std::filesystem::exists(target))
        return target;

    std::wstring stem = original_file.stem().wstring();
    std::wstring ext = original_file.extension().wstring();

    // Если файл без расширения, ext будет пустым
    int counter = 1;
    while (true)
    {
        std::wstring new_name = stem + L" (" + std::to_wstring(counter) + L")" + ext;
        target = dest_dir / new_name;

        if (!std::filesystem::exists(target))
            return target;
        counter++;
    }
}

// Перемещает один файл в нужную папку
export bool MoveFileToCategory(const std::filesystem::path& file, const std::filesystem::path& downloads) {
    std::wstring ext = file.extension().wstring();

    // 1. Определяем папку
    std::wstring_view folder_name = FindFolderForExtension(ext);
    if (folder_name.empty())
        folder_name = L"+_Other";

    std::filesystem::path dest_dir = downloads / folder_name;

    // 2. Если файл уже лежит в этой папке — ничего не делаем
    if (file.parent_path() == dest_dir)
        return false;

    // 3. Создаем папку, если ее нет
    std::error_code ec;
    if (!std::filesystem::exists(dest_dir))
    {
        std::filesystem::create_directories(dest_dir, ec);
        if (ec)
        {
            std::println(std::cerr, "Ошибка создания папки {}: {}", std::string(folder_name.begin(), folder_name.end()), ec.message());
            return false;
        }
        std::println("  [Создана папка] {}", std::string(folder_name.begin(), folder_name.end()));
    }

    // 4. Разрешаем конфликты имен
    std::filesystem::path dest_path = GetUniquePath(dest_dir, file);

    // 5. Перемещаем (используем MoveFileExW с флагом COPY_ALLOWED для надежности между дисками)
    if (MoveFileExW(file.c_str(), dest_path.c_str(), MOVEFILE_COPY_ALLOWED))
    {
        std::println("  [Перемещен] {} -> {}",
            file.filename().string(),
            std::string(folder_name.begin(), folder_name.end()));
        return true;
    }
    else
    {
        std::println(std::cerr, "  [Ошибка] Не удалось переместить {}", file.filename().string());
        return false;
    }
}

// Перемещает файл в папку дубликатов
export bool MoveFileToDuplicate(const std::filesystem::path& file, const std::filesystem::path& downloads) {
    std::filesystem::path dest_dir = downloads / kDuplicateFolder;

    // 1. Создаем папку +0_Duplicate, если ее нет
    std::error_code ec;
    if (!std::filesystem::exists(dest_dir)) {
        std::filesystem::create_directories(dest_dir, ec);
        if (ec)
        {
            std::println(std::cerr, "Ошибка создания папки дубликатов: {}", ec.message());
            return false;
        }
        std::println("  [Создана папка] +0_Duplicate");
    }

    // 2. Разрешаем конфликты имен
    std::filesystem::path dest_path = GetUniquePath(dest_dir, file);

    // 3. Перемещаем
    if (MoveFileExW(file.c_str(), dest_path.c_str(), MOVEFILE_COPY_ALLOWED))
    {
        std::println("  [Дубликат] {} -> +0_Duplicate", file.filename().string());
        return true;
    }
    else
    {
        std::println(std::cerr, "  [Ошибка] Не удалось переместить дубликат {}", file.filename().string());
        return false;
    }
}