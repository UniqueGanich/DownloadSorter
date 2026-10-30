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

    DWORD objSize = 0, hashLen = 0, result = 0;

    // Проверяем статус получения свойств
    if (!BCRYPT_SUCCESS(BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objSize, sizeof(DWORD), &result, 0))) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }
    if (!BCRYPT_SUCCESS(BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (PUCHAR)&hashLen, sizeof(DWORD), &result, 0))) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    std::vector<unsigned char> objBuf(objSize);
    BCRYPT_HASH_HANDLE hHash = nullptr;
    if (!BCRYPT_SUCCESS(BCryptCreateHash(hAlg, &hHash, objBuf.data(), objSize, nullptr, 0, 0))) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    std::ifstream in(file, std::ios::binary);
    if (!in) {
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    std::vector<char> buffer(8192);
    bool read_error = false;
    while (in.read(buffer.data(), buffer.size()) || in.gcount() > 0) {
        // Проверяем, что данные приняты в хэш
        if (!BCRYPT_SUCCESS(BCryptHashData(hHash, (PUCHAR)buffer.data(), (ULONG)in.gcount(), 0))) {
            read_error = true;
            break;
        }
    }

    if (read_error) {
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    // Проверяем успешное завершение
    if (!BCRYPT_SUCCESS(BCryptFinishHash(hHash, outHash.data(), (ULONG)outHash.size(), 0))) {
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return true;
}

export std::optional<std::array<unsigned char, 32>> GetFullHash(const std::filesystem::path& file) {
    std::array<unsigned char, 32> hashResult{};
    if (ComputeHash(file, hashResult)) {
        return hashResult;
    }
    return std::nullopt;
}