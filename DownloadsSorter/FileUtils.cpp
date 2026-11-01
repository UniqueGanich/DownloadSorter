module;
#include <windows.h>
#include <chrono>

export module FileUtils;
import std;
import Data;
import Console;

export enum class FileStatus {
    Eligible,   // Подходит
    Temporary,  // Временный (.crdownload, .part)
    Locked,     // Качается прямо сейчас (изменен менее 5 сек назад)
    Ignored,    // Игнорируется (системный мусор)
    Error       // Ошибка доступа
};

export FileStatus CheckFileStatus(const std::filesystem::path& p) {
    const std::wstring ext = p.extension().wstring();
    const std::wstring filename = p.filename().wstring();

    if (_wcsicmp(filename.c_str(), L"desktop.ini") == 0 ||
        _wcsicmp(filename.c_str(), L"thumbs.db") == 0)
        return FileStatus::Ignored;

    DWORD attrs = GetFileAttributesW(GetWinApiPath(p).c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)))
        return FileStatus::Ignored;

    if (IsTemporaryExtension(ext)) return FileStatus::Temporary;

    std::error_code ec;
    auto ftime = std::filesystem::last_write_time(p, ec);
    if (ec) return FileStatus::Error;

    auto age = std::filesystem::file_time_type::clock::now() - ftime;

    if (age <= std::chrono::seconds(5)) {
        return FileStatus::Locked;
    }

    return FileStatus::Eligible;
}