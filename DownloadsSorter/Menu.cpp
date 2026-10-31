module;
#include <windows.h>
#include <conio.h>
#include <string>

export module Menu;
import std;
import Console;
import Config;
import Localization;

inline void ClearScreen() { system("cls"); }

inline int GetSafeInt() {
    int val;
    while (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::println("{} {}: {}", RED, Loc("Некорректный ввод. Введите число", "Invalid input. Enter a number"), RESET);
    }
    std::cin.ignore(10000, '\n');
    return val;
}

inline void WaitAnyKey() {
    std::println("\n{}", Loc("Нажмите любую клавишу...", "Press any key..."));
    _getch();
}

export void ShowMainMenu(AppConfig& cfg) {
    ClearScreen();
    std::println("{}{}========================================{}", RESET, CYAN, RESET);
    std::println("{}{}       {}    {}{}", RESET, BOLD,
        Loc("DOWNLOADS SORTER - ГЛАВНОЕ МЕНЮ", "DOWNLOADS SORTER - MAIN MENU"),
        CYAN, RESET, RESET);
    std::println("{}{}========================================{}\n", RESET, CYAN, RESET);

    std::string pathStr = cfg.targetFolder.empty() ? Loc("По умолчанию (Загрузки)", "Default (Downloads)") : ToUtf8(cfg.targetFolder);
    std::string autoStr = (cfg.autoRunMode == 0) ? Loc("Выкл", "Off") :
        (cfg.autoRunMode == 1) ? Loc("При запуске", "On startup") :
        (Loc("Каждые ", "Every ") + std::to_string(cfg.customTimeHours) + Loc(" ч.", " hrs"));

    std::println("{}1.{} {}", GREEN, RESET, Loc("Запустить сортировщик", "Run sorter"));
    std::println("{}2.{} {}  [{}: {}{}{}]", YELLOW, RESET, Loc("Режим автозапуска", "Auto-run mode"), Loc("Сейчас", "Now"), CYAN, autoStr, RESET);
    std::println("{}3.{} {}   [{}: {}{}{}]", YELLOW, RESET, Loc("Папка сортировки", "Sorting folder"), Loc("Сейчас", "Now"), CYAN, pathStr, RESET);
    std::println("{}0.{} {}\n", RED, RESET, Loc("Выход", "Exit"));
    std::print("{}", Loc("Выберите действие: ", "Select action: "));
}

export void ShowAutoRunMenu(AppConfig& cfg) {
    ClearScreen();
    std::println("{}--- {} ---{}\n", BOLD, Loc("НАСТРОЙКА АВТОЗАПУСКА", "AUTO-RUN SETTINGS"), CYAN, RESET);
    std::println("1. {}", Loc("Выключить", "Turn off"));
    std::println("2. {}", Loc("Включать при запуске", "Enable on startup"));
    std::println("3. {}", Loc("Запускать по таймеру (в часах)", "Run on timer (in hours)"));
    std::println("0. {}\n", Loc("Назад", "Back"));
    std::print("{}", Loc("Выбор: ", "Choice: "));

    char choice = _getch();
    if (choice == '1') {
        cfg.autoRunMode = 0;
        ApplyAutoRunSettings(cfg);
        std::println("\n{}{}", GREEN, Loc("Автозапуск отключен.", "Auto-run disabled."));
        WaitAnyKey();
    }
    else if (choice == '2') {
        cfg.autoRunMode = 1;
        ApplyAutoRunSettings(cfg);
        std::println("\n{}{}", GREEN, Loc("Задача создана! Программа будет запускаться при входе в систему.", "Task created! Program will run on system startup."));
        WaitAnyKey();
    }
    else if (choice == '3') {
        ClearScreen();
        std::print("{}", Loc("Введите интервал в часах (например, 2): ", "Enter interval in hours (e.g., 2): "));
        cfg.customTimeHours = GetSafeInt();
        if (cfg.customTimeHours > 0) {
            cfg.autoRunMode = 2;
            ApplyAutoRunSettings(cfg);
            std::println("\n{}{} {} {}.", GREEN, Loc("Задача создана! Программа будет запускаться каждые", "Task created! Program will run every"), cfg.customTimeHours, Loc("ч.", "hrs."));
        }
        else {
            std::println("\n{}{}", RED, Loc("[Ошибка] Время должно быть больше 0.", "[Error] Time must be greater than 0."));
        }
        WaitAnyKey();
    }
    SaveConfig(cfg);
}

export void ShowFolderMenu(AppConfig& cfg) {
    ClearScreen();
    std::println("{}--- {} ---{}\n", BOLD, Loc("ВЫБОР ПАПКИ", "FOLDER SELECTION"), CYAN, RESET);
    std::println("1. {}", Loc("Папка «Загрузки» по умолчанию", "Default Downloads folder"));
    std::println("2. {}", Loc("Указать свой путь", "Specify custom path"));
    std::println("0. {}\n", Loc("Назад", "Back"));
    std::print("{}", Loc("Выбор: ", "Choice: "));

    char choice = _getch();
    if (choice == '1') {
        cfg.targetFolder = L"";
        SaveConfig(cfg);
    }
    else if (choice == '2') {
        ClearScreen();
        std::println("{}\n{}\n", Loc("Введите полный путь к папке.", "Enter full path to folder."), Loc("Пример: C:\\Users\\User\\Downloads", "Example: C:\\Users\\User\\Downloads"));
        std::print("{}", Loc("Путь: ", "Path: "));
        std::string input;
        std::getline(std::cin, input);

        int size = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, nullptr, 0);
        std::wstring wPath(size - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, wPath.data(), size);

        std::error_code ec;
        if (std::filesystem::is_directory(wPath, ec)) {
            cfg.targetFolder = wPath;
            SaveConfig(cfg);
            std::println("\n{}{}", GREEN, Loc("Путь сохранен!", "Path saved!"));
        }
        else {
            std::println("\n{}{}", RED, Loc("[Ошибка] Папка не найдена!", "[Error] Folder not found!"));
        }
        WaitAnyKey();
    }
}