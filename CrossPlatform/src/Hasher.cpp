#include "Hasher.h"

#ifdef _WIN32
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#else
#include <fcntl.h>
#include <openssl/sha.h>
#include <unistd.h>
#endif

#include <fstream>
#include <iostream>
#include <memory>

constexpr size_t QUICK_BUF_SIZE = 1024 * 1024;
constexpr size_t FULL_BUF_SIZE = 4 * 1024 * 1024;

#ifdef _WIN32
static BCRYPT_HASH_HANDLE CreateHasher(BCRYPT_ALG_HANDLE& hAlg, std::vector<unsigned char>& objBuf) {
    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0))) return nullptr;
    DWORD objSize = 0, result = 0;
    BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objSize, sizeof(DWORD), &result, 0);
    objBuf.resize(objSize);
    BCRYPT_HASH_HANDLE hHash = nullptr;
    if (!BCRYPT_SUCCESS(BCryptCreateHash(hAlg, &hHash, objBuf.data(), objSize, nullptr, 0, 0))) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return nullptr;
    }
    return hHash;
}

std::optional<std::array<unsigned char, 32>> GetQuickHash(const std::filesystem::path& file) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    std::vector<unsigned char> objBuf;
    BCRYPT_HASH_HANDLE hHash = CreateHasher(hAlg, objBuf);
    if (!hHash) return std::nullopt;

    HANDLE hFile = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) { BCryptDestroyHash(hHash); BCryptCloseAlgorithmProvider(hAlg, 0); return std::nullopt; }

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
    BCryptDestroyHash(hHash); BCryptCloseAlgorithmProvider(hAlg, 0);
    return ok ? std::optional(outHash) : std::nullopt;
}

std::optional<std::array<unsigned char, 32>> GetFullHash(const std::filesystem::path& file) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    std::vector<unsigned char> objBuf;
    BCRYPT_HASH_HANDLE hHash = CreateHasher(hAlg, objBuf);
    if (!hHash) return std::nullopt;

    HANDLE hFile = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) { BCryptDestroyHash(hHash); BCryptCloseAlgorithmProvider(hAlg, 0); return std::nullopt; }

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
    BCryptDestroyHash(hHash); BCryptCloseAlgorithmProvider(hAlg, 0);
    return ok ? std::optional(outHash) : std::nullopt;
}

#else

#include <openssl/evp.h>

std::optional<std::array<unsigned char, 32>> GetQuickHash(const std::filesystem::path& file) {
    std::ifstream in(file, std::ios::binary);
    if (!in) return std::nullopt;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);

    std::vector<unsigned char> buffer(QUICK_BUF_SIZE);

    in.read(reinterpret_cast<char*>(buffer.data()), QUICK_BUF_SIZE);
    std::streamsize bytesRead = in.gcount();
    if (bytesRead > 0) EVP_DigestUpdate(ctx, buffer.data(), bytesRead);

    in.seekg(0, std::ios::end);
    std::streamsize fileSize = in.tellg();
    if (fileSize > static_cast<std::streamsize>(QUICK_BUF_SIZE * 2)) {
        in.seekg(-static_cast<std::streamoff>(QUICK_BUF_SIZE), std::ios::end);
        in.read(reinterpret_cast<char*>(buffer.data()), QUICK_BUF_SIZE);
        bytesRead = in.gcount();
        if (bytesRead > 0) EVP_DigestUpdate(ctx, buffer.data(), bytesRead);
    }

    std::array<unsigned char, 32> outHash{};
    unsigned int len = 0;
    EVP_DigestFinal_ex(ctx, outHash.data(), &len);
    EVP_MD_CTX_free(ctx);

    return outHash;
}

std::optional<std::array<unsigned char, 32>> GetFullHash(const std::filesystem::path& file) {
    std::ifstream in(file, std::ios::binary);
    if (!in) return std::nullopt;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);

    std::vector<unsigned char> buffer(FULL_BUF_SIZE);
    while (in.read(reinterpret_cast<char*>(buffer.data()), FULL_BUF_SIZE) || in.gcount() > 0) {
        EVP_DigestUpdate(ctx, buffer.data(), in.gcount());
    }

    std::array<unsigned char, 32> outHash{};
    unsigned int len = 0;
    EVP_DigestFinal_ex(ctx, outHash.data(), &len);
    EVP_MD_CTX_free(ctx);

    return outHash;
}
#endif