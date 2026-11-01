module;
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

export module Hasher;
import std;
import Console;

static BCRYPT_HASH_HANDLE CreateHasher(BCRYPT_ALG_HANDLE& hAlg, std::vector<unsigned char>& objBuf) {
    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0)))
        return nullptr;

    DWORD objSize = 0, result = 0;
    if (!BCRYPT_SUCCESS(BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objSize, sizeof(DWORD), &result, 0))) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return nullptr;
    }

    objBuf.resize(objSize);
    BCRYPT_HASH_HANDLE hHash = nullptr;
    if (!BCRYPT_SUCCESS(BCryptCreateHash(hAlg, &hHash, objBuf.data(), objSize, nullptr, 0, 0))) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return nullptr;
    }
    return hHash;
}

export std::optional<std::array<unsigned char, 32>> GetQuickHash(const std::filesystem::path& file) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    std::vector<unsigned char> objBuf;
    BCRYPT_HASH_HANDLE hHash = CreateHasher(hAlg, objBuf);
    if (!hHash) return std::nullopt;

    HANDLE hFile = CreateFileW(GetWinApiPath(file).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        BCryptDestroyHash(hHash); BCryptCloseAlgorithmProvider(hAlg, 0);
        return std::nullopt;
    }

    constexpr DWORD QUICK_BUF_SIZE = 1024 * 1024;
    auto buffer = std::make_unique<unsigned char[]>(QUICK_BUF_SIZE);
    DWORD bytesRead = 0;
    bool ok = true;

    if (ReadFile(hFile, buffer.get(), QUICK_BUF_SIZE, &bytesRead, nullptr) && bytesRead > 0) {
        ok = BCRYPT_SUCCESS(BCryptHashData(hHash, buffer.get(), bytesRead, 0));
    }
    else { ok = false; }

    if (ok) {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(hFile, &fileSize) && fileSize.QuadPart > (QUICK_BUF_SIZE * 2LL)) {
            LARGE_INTEGER li = {};
            li.QuadPart = -(LONGLONG)QUICK_BUF_SIZE;
            if (SetFilePointerEx(hFile, li, nullptr, FILE_END)) {
                if (ReadFile(hFile, buffer.get(), QUICK_BUF_SIZE, &bytesRead, nullptr) && bytesRead > 0) {
                    ok = BCRYPT_SUCCESS(BCryptHashData(hHash, buffer.get(), bytesRead, 0));
                }
                else { ok = false; }
            }
            else { ok = false; }
        }
    }

    CloseHandle(hFile);

    std::array<unsigned char, 32> outHash{};
    if (ok) ok = BCRYPT_SUCCESS(BCryptFinishHash(hHash, outHash.data(), (ULONG)outHash.size(), 0));

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return ok ? std::optional(outHash) : std::nullopt;
}

export std::optional<std::array<unsigned char, 32>> GetFullHash(const std::filesystem::path& file) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    std::vector<unsigned char> objBuf;
    BCRYPT_HASH_HANDLE hHash = CreateHasher(hAlg, objBuf);
    if (!hHash) return std::nullopt;

    HANDLE hFile = CreateFileW(GetWinApiPath(file).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        BCryptDestroyHash(hHash); BCryptCloseAlgorithmProvider(hAlg, 0);
        return std::nullopt;
    }

    constexpr DWORD FULL_BUF_SIZE = 4 * 1024 * 1024;
    auto buffer = std::make_unique<unsigned char[]>(FULL_BUF_SIZE);
    DWORD bytesRead = 0;
    bool ok = true;

    while (ok) {
        if (!ReadFile(hFile, buffer.get(), FULL_BUF_SIZE, &bytesRead, nullptr)) { ok = false; break; }
        if (bytesRead == 0) break;
        if (!BCRYPT_SUCCESS(BCryptHashData(hHash, buffer.get(), bytesRead, 0))) ok = false;
    }
    CloseHandle(hFile);

    std::array<unsigned char, 32> outHash{};
    if (ok) ok = BCRYPT_SUCCESS(BCryptFinishHash(hHash, outHash.data(), (ULONG)outHash.size(), 0));

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return ok ? std::optional(outHash) : std::nullopt;
}