#include "Localization.h"
#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <windows.h>
bool IsRussian() {
    LANGID langId = GetUserDefaultUILanguage();
    return PRIMARYLANGID(langId) == LANG_RUSSIAN;
}
#else
bool IsRussian() {
    const char* lang = std::getenv("LANG");
    if (lang && (std::string(lang).find("ru") == 0 || std::string(lang).find("_RU") != std::string::npos)) return true;
    return false;
}
#endif

const char* Loc(const char* ru, const char* en) {
    return IsRussian() ? ru : en;
}