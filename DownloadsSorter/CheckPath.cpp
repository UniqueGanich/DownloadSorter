module;
#include <shlobj.h>

export module CheckPath;
import std;

export std::optional<std::filesystem::path> getDownloadsPath() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
        return std::nullopt;
    const bool need_uninit = SUCCEEDED(hr);

    PWSTR raw_path = nullptr;
    hr = SHGetKnownFolderPath(FOLDERID_Downloads, KF_FLAG_DEFAULT_PATH, nullptr, &raw_path);

    std::optional<std::filesystem::path> result;
    if (SUCCEEDED(hr) && raw_path != nullptr)
        result = std::filesystem::path(raw_path);

    if (raw_path != nullptr) CoTaskMemFree(raw_path);
    if (need_uninit) CoUninitialize();

    return result;
}