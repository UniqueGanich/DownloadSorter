module;
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

export module Hasher;
import std;

static bool ComputeHash(const std::filesystem::path& file, std::array<unsigned char, 32>& outHash)
{
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0)))
        return false;

    DWORD objSize = 0, hashLen = 0, result = 0;

    // Проверяем статус получения свойств
    if (!BCRYPT_SUCCESS(BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objSize, sizeof(DWORD), &result, 0)))
    {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }
    if (!BCRYPT_SUCCESS(BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (PUCHAR)&hashLen, sizeof(DWORD), &result, 0)))
    {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    std::vector<unsigned char> objBuf(objSize);
    BCRYPT_HASH_HANDLE hHash = nullptr;
    if (!BCRYPT_SUCCESS(BCryptCreateHash(hAlg, &hHash, objBuf.data(), objSize, nullptr, 0, 0)))
    {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    // ИСПОЛЬЗУЕМ WINAPI ВМЕСТО std::ifstream ДЛЯ ПОДДЕРЖКИ КИРИЛЛИЦЫ
    HANDLE hFile = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    std::vector<unsigned char> buffer(8192);
    DWORD bytesRead = 0;
    bool read_error = false;

    // Цикл чтения с защитой от сбоев диска
    while (true)
    {
        if (!ReadFile(hFile, buffer.data(), (DWORD)buffer.size(), &bytesRead, nullptr))
        {
            read_error = true;
            break;
        }
        if (bytesRead == 0)
            break;

        if (!BCRYPT_SUCCESS(BCryptHashData(hHash, (PUCHAR)buffer.data(), bytesRead, 0))) 
        {
            read_error = true;
            break;
        }
    }

    CloseHandle(hFile);

    if (read_error)
    {
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    if (!BCRYPT_SUCCESS(BCryptFinishHash(hHash, outHash.data(), (ULONG)outHash.size(), 0)))
    {
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return false;
    }

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return true;
}

export std::optional<std::array<unsigned char, 32>> GetFullHash(const std::filesystem::path& file)
{
    std::array<unsigned char, 32> hashResult{};
    if (ComputeHash(file, hashResult))
        return hashResult;
    return std::nullopt;
}