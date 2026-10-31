#include <Windows.h>
#include <conio.h>

import std;
import Sorter;
import Menu;
import Config;
import CheckPath;
import Console;

int main(int argc, char* argv[]) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);

    bool isSilent = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string_view(argv[i]) == "--silent") {
            isSilent = true;
        }
    }

    AppConfig cfg = LoadConfig();
    std::filesystem::path target;

    if (cfg.targetFolder.empty()) {
        auto opt = getDownloadsPath();
        if (opt) target = *opt;
        else { if (!isSilent) std::println("Ошибка получения пути к Загрузкам!"); return 1; }
    }
    else {
        target = cfg.targetFolder;
    }

    if (isSilent) {
        sorter(target);
        return 0;
    }

    bool exitApp = false;
    while (!exitApp) {
        ShowMainMenu(cfg);
        char choice = _getch();

        switch (choice) {
        case '1': {
            sorter(target);
            std::println("\nНажмите любую клавишу для возврата в меню...");
            _getch();
            break;
        }
        case '2':
            ShowAutoRunMenu(cfg);
            break;
        case '3':
            ShowFolderMenu(cfg);
            if (cfg.targetFolder.empty()) {
                auto opt = getDownloadsPath();
                if (opt) target = *opt;
            }
            else {
                target = cfg.targetFolder;
            }
            break;
        case '0':
            exitApp = true;
            break;
        }
    }

    return 0;
}