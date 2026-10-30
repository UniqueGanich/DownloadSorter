module;
#include <windows.h>

export module Console;
import std;

export std::string ToUtf8(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

export inline constexpr std::string_view RESET = "\033[0m";
export inline constexpr std::string_view RED = "\033[31m";
export inline constexpr std::string_view GREEN = "\033[32m";
export inline constexpr std::string_view YELLOW = "\033[33m";
export inline constexpr std::string_view CYAN = "\033[36m";
export inline constexpr std::string_view BOLD = "\033[1m";