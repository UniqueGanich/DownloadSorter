module;
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

export module Hasher;
import std;

static bool ComputeHash(const std::filesystem::path& file, std::array<unsigned char, 32>& outHash) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0)))
        return false;

    DWORD objSize = 0, result = 0;
    if (!BCRYPT_SUCCESS(BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objSize, sizeof(DWORD), &result, 0))) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    std::vector<unsigned char> objBuf(objSize);
    BCRYPT_HASH_HANDLE hHash = nullptr;
    if (!BCRYPT_SUCCESS(BCryptCreateHash(hAlg, &hHash, objBuf.data(), objSize, nullptr, 0, 0))) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    // WinAPI вместо std::ifstream — поддержка кириллических путей
    HANDLE hFile = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    std::vector<unsigned char> buffer(8192);
    DWORD bytesRead = 0;
    bool ok = true;
    while (ok) {
        if (!ReadFile(hFile, buffer.data(), (DWORD)buffer.size(), &bytesRead, nullptr)) { ok = false; break; }
        if (bytesRead == 0) break;
        if (!BCRYPT_SUCCESS(BCryptHashData(hHash, buffer.data(), bytesRead, 0))) ok = false;
    }
    CloseHandle(hFile);

    if (ok)
        ok = BCRYPT_SUCCESS(BCryptFinishHash(hHash, outHash.data(), (ULONG)outHash.size(), 0));

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return ok;
}

export std::optional<std::array<unsigned char, 32>> GetFullHash(const std::filesystem::path& file) {
    std::array<unsigned char, 32> hash{};
    return ComputeHash(file, hash) ? std::optional(hash) : std::nullopt;
}