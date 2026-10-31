module;
#include <windows.h>

export module Localization;
import std;

export bool IsRussian() {
    LANGID langId = GetUserDefaultUILanguage();
    WORD primaryLang = PRIMARYLANGID(langId);
    return primaryLang == LANG_RUSSIAN;
}

export const char* Loc(const char* ru, const char* en) {
    return IsRussian() ? ru : en;
}