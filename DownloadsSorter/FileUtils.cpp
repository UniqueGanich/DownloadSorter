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
    Locked,     // Занят другой программой или качается прямо сейчас
    Ignored,    // Игнорируется (системный мусор)
    Error       // Ошибка доступа
};

export bool IsFileLockedByOS(const std::filesystem::path& p) {
    HANDLE h = CreateFileW(GetWinApiPath(p).c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return true;
    CloseHandle(h);
    return false;
}

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
    if (ec) return FileStatus::Locked;

    auto age = std::filesystem::file_time_type::clock::now() - ftime;

    if (age > std::chrono::seconds(5)) {
        return FileStatus::Eligible;
    }

    if (IsFileLockedByOS(p))  return FileStatus::Locked;

    return FileStatus::Locked;
}