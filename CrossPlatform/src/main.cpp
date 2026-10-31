#include "Config.h"
#include "Console.h"
#include "Localization.h"
#include "Menu.h"
#include "Platform.h"
#include "Sorter.h"
#include <filesystem>
#include <iostream>

int main(int argc, char* argv[]) {
    InitConsole();

    bool isSilent = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string_view(argv[i]) == "--silent") {
            isSilent = true;
        }
    }

    AppConfig cfg = LoadConfig();
    std::filesystem::path target;

    if (cfg.targetFolder.empty()) {
        target = GetDownloadPath();
        if (target.empty()) {
            if (!isSilent) std::cerr << RED << Loc("[Ошибка] Не удалось найти папку Загрузки!", "[Error] Could not find Downloads folder!") << RESET << std::endl;
            return 1;
        }
    }
    else {
        target = cfg.targetFolder;
    }

    if (isSilent) {
        SortDownloads(target);
        return 0;
    }

    bool exitApp = false;
    while (!exitApp) {
        ShowMainMenu(cfg, target);
        char choice = GetChar();
        switch (choice) {
        case '1':
            SortDownloads(target);
            std::cout << "\n" << Loc("Нажмите любую клавишу для возврата в меню...", "Press any key to return to menu...") << std::endl;
            GetChar();
            break;
        case '2':
            ShowAutoRunMenu(cfg);
            break;
        case '3':
            ShowFolderMenu(cfg);
            if (cfg.targetFolder.empty()) {
                target = GetDownloadPath();
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