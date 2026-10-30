module;
#include <shlobj.h>

export module CheckPath;
import std;

export std::optional<std::filesystem::path> getDownloadsPath() {

    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) return std::nullopt;

    PWSTR raw_path = nullptr;
    hr = SHGetKnownFolderPath(FOLDERID_Downloads, KF_FLAG_DEFAULT_PATH, nullptr, &raw_path);
    std::optional<std::filesystem::path> result = std::nullopt;
    if (SUCCEEDED(hr) && raw_path != nullptr) result = std::filesystem::path(raw_path);
    if (raw_path != nullptr) CoTaskMemFree(raw_path);
    CoUninitialize();

    return result;
}