module;
#include <windows.h>

export module Console;
import std;

export std::string ToUtf8(std::wstring_view wstr) {
    if (wstr.empty()) return {};

    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(),
        nullptr, 0, nullptr, nullptr);
    std::string out(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(),
        out.data(), size, nullptr, nullptr);
    return out;
}

export inline std::wstring GetWinApiPath(const std::filesystem::path& p) {
    std::wstring s = p.wstring();
    if (s.length() > 240 && s.rfind(L"\\\\?\\", 0) != 0) {
    return L"\\\\?\\" + s;
    }
return s;
}

export inline constexpr std::string_view RESET = "\033[0m";
export inline constexpr std::string_view RED = "\033[31m";
export inline constexpr std::string_view GREEN = "\033[32m";
export inline constexpr std::string_view YELLOW = "\033[33m";
export inline constexpr std::string_view CYAN = "\033[36m";
export inline constexpr std::string_view BOLD = "\033[1m";