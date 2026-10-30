module;
#include <windows.h>

export module FileUtils;
import std;
import Data;

export enum class FileStatus
{
    Eligible,   // Подходит
    Temporary,  // Временный (.crdownload)
    TooFresh,   // Слишком новый (<24ч)
    Locked,     // Занят другой программой
    Ignored,    // Игнорируется
    Error       // Ошибка доступа
};

export bool IsOlderThan24Hours(const std::filesystem::path& p)
{
    std::error_code ec;
    auto ftime = std::filesystem::last_write_time(p, ec);
    if (ec) return false;
    auto now = std::filesystem::file_time_type::clock::now();
    return (now - ftime) > std::chrono::hours(24);
}

export bool IsFileLocked(const std::filesystem::path& p)
{
    HANDLE hFile = CreateFileW(p.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return true;
    CloseHandle(hFile);
    return false;
}

export FileStatus CheckFileStatus(const std::filesystem::path& p)
{
    std::wstring ext = p.extension().wstring();
    std::wstring filename = p.filename().wstring();

    if (_wcsicmp(filename.c_str(), L"desktop.ini") == 0 ||
        _wcsicmp(filename.c_str(), L"thumbs.db") == 0)
        return FileStatus::Ignored;

    DWORD attrs = GetFileAttributesW(p.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES)
        if ((attrs & FILE_ATTRIBUTE_HIDDEN) || (attrs & FILE_ATTRIBUTE_SYSTEM))
            return FileStatus::Ignored;

    if (IsTemporaryExtension(ext)) return FileStatus::Temporary;
    if (!IsOlderThan24Hours(p))    return FileStatus::TooFresh;
    if (IsFileLocked(p))           return FileStatus::Locked;

    return FileStatus::Eligible;
}