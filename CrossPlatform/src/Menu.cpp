#include "Console.h"
#include "Localization.h"
#include "Menu.h"
#include "Platform.h"
#include <iostream>
#include <limits>
#include <string>

static void ClearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static int GetSafeInt() {
    int val;
    while (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << RED << Loc("Некорректный ввод. Введите число: ", "Invalid input. Enter a number: ") << RESET;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return val;
}

static void WaitAnyKey() {
    std::cout << "\n" << Loc("Нажмите любую клавишу...", "Press any key...") << std::endl;
    GetChar();
}

void ShowMainMenu(AppConfig& cfg, const std::filesystem::path& currentTarget) {
    ClearScreen();
    std::cout << RESET << CYAN << "========================================" << RESET << std::endl;
    std::cout << RESET << BOLD << "       " << Loc("DOWNLOADS SORTER - ГЛАВНОЕ МЕНЮ", "DOWNLOADS SORTER - MAIN MENU") << "    " << RESET << std::endl;
    std::cout << RESET << CYAN << "========================================\n" << RESET << std::endl;

    std::string pathStr = cfg.targetFolder.empty() ? Loc("По умолчанию (Загрузки)", "Default (Downloads)") : cfg.targetFolder.string();
    std::string autoStr = (cfg.autoRunMode == 0) ? Loc("Выкл", "Off") :
        (cfg.autoRunMode == 1) ? Loc("При запуске", "On startup") :
        (Loc("Каждые ", "Every ") + std::to_string(cfg.customTimeHours) + Loc(" ч.", " hrs"));

    std::cout << GREEN << "1. " << RESET << Loc("Запустить сортировщик", "Run sorter") << std::endl;
    std::cout << YELLOW << "2. " << RESET << Loc("Режим автозапуска", "Auto-run mode") << "  [" << Loc("Сейчас", "Now") << ": " << CYAN << autoStr << RESET << "]" << std::endl;
    std::cout << YELLOW << "3. " << RESET << Loc("Папка сортировки", "Sorting folder") << "   [" << Loc("Сейчас", "Now") << ": " << CYAN << pathStr << RESET << "]" << std::endl;
    std::cout << RED << "0. " << RESET << Loc("Выход", "Exit") << "\n" << std::endl;
    std::cout << Loc("Выберите действие: ", "Select action: ");
}

void ShowAutoRunMenu(AppConfig& cfg) {
    ClearScreen();
    std::cout << BOLD << "--- " << Loc("НАСТРОЙКА АВТОЗАПУСКА", "AUTO-RUN SETTINGS") << " ---\n" << CYAN << RESET << std::endl;
    std::cout << "1. " << Loc("Выключить", "Turn off") << std::endl;
    std::cout << "2. " << Loc("Включать при запуске", "Enable on startup") << std::endl;
    std::cout << "3. " << Loc("Запускать по таймеру (в часах)", "Run on timer (in hours)") << std::endl;
    std::cout << "0. " << Loc("Назад", "Back") << "\n" << std::endl;
    std::cout << Loc("Выбор: ", "Choice: ");

    char choice = GetChar();
    if (choice == '1') {
        cfg.autoRunMode = 0;
        ApplyAutoRunSettings(cfg);
        std::cout << "\n" << GREEN << Loc("Автозапуск отключен.", "Auto-run disabled.") << RESET << std::endl;
        WaitAnyKey();
    }
    else if (choice == '2') {
        cfg.autoRunMode = 1;
        ApplyAutoRunSettings(cfg);
        std::cout << "\n" << GREEN << Loc("Задача создана! Программа будет запускаться при входе в систему.", "Task created! Program will run on system startup.") << RESET << std::endl;
        WaitAnyKey();
    }
    else if (choice == '3') {
        ClearScreen();
        std::cout << Loc("Введите интервал в часах (например, 2): ", "Enter interval in hours (e.g., 2): ");
        cfg.customTimeHours = GetSafeInt();
        if (cfg.customTimeHours > 0) {
            cfg.autoRunMode = 2;
            ApplyAutoRunSettings(cfg);
            std::cout << "\n" << GREEN << Loc("Задача создана! Программа будет запускаться каждые ", "Task created! Program will run every ") << cfg.customTimeHours << Loc(" ч.", " hrs.") << RESET << std::endl;
        }
        else {
            std::cout << "\n" << RED << Loc("[Ошибка] Время должно быть больше 0.", "[Error] Time must be greater than 0.") << RESET << std::endl;
        }
        WaitAnyKey();
    }
    SaveConfig(cfg);
}

void ShowFolderMenu(AppConfig& cfg) {
    ClearScreen();
    std::cout << BOLD << "--- " << Loc("ВЫБОР ПАПКИ", "FOLDER SELECTION") << " ---\n" << CYAN << RESET << std::endl;
    std::cout << "1. " << Loc("Папка «Загрузки» по умолчанию", "Default Downloads folder") << std::endl;
    std::cout << "2. " << Loc("Указать свой путь", "Specify custom path") << std::endl;
    std::cout << "0. " << Loc("Назад", "Back") << "\n" << std::endl;
    std::cout << Loc("Выбор: ", "Choice: ");

    char choice = GetChar();
    if (choice == '1') {
        cfg.targetFolder = "";
        SaveConfig(cfg);
    }
    else if (choice == '2') {
        ClearScreen();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << Loc("Введите полный путь к папке.", "Enter full path to folder.") << "\n";
        std::cout << Loc("Пример: C:\\Users\\User\\Downloads или /home/user/Downloads", "Example: C:\\Users\\User\\Downloads or /home/user/Downloads") << "\n\n";
        std::cout << Loc("Путь: ", "Path: ");
        std::string input;
        std::getline(std::cin, input);

        std::error_code ec;
        if (std::filesystem::is_directory(input, ec)) {
            cfg.targetFolder = input;
            SaveConfig(cfg);
            std::cout << "\n" << GREEN << Loc("Путь сохранен!", "Path saved!") << RESET << std::endl;
        }
        else {
            std::cout << "\n" << RED << Loc("[Ошибка] Папка не найдена!", "[Error] Folder not found!") << RESET << std::endl;
        }
        WaitAnyKey();
    }
}