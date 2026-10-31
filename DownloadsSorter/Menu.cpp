module;
#include <windows.h>
#include <conio.h>
#include <string>

export module Menu;
import std;
import Console;
import Config;

inline void ClearScreen() { system("cls"); }

inline int GetSafeInt() {
    int val;
    while (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::println("{}Некорректный ввод. Введите число: {}", RED, RESET);
    }
    std::cin.ignore(10000, '\n');
    return val;
}

inline void WaitAnyKey() {
    std::println("\nНажмите любую клавишу...");
    _getch();
}

export void ShowMainMenu(AppConfig& cfg) {
    ClearScreen();
    std::println("{}{}========================================{}", RESET, CYAN, RESET);
    std::println("{}{}       DOWNLOADS SORTER - ГЛАВНОЕ МЕНЮ    {}{}", RESET, BOLD, CYAN, RESET, RESET);
    std::println("{}{}========================================{}\n", RESET, CYAN, RESET);

    std::string pathStr = cfg.targetFolder.empty() ? "По умолчанию (Загрузки)" : ToUtf8(cfg.targetFolder);
    std::string autoStr = (cfg.autoRunMode == 0) ? "Выкл" : (cfg.autoRunMode == 1) ? "При запуске" : ("Каждые " + std::to_string(cfg.customTimeHours) + " ч.");

    std::println("{}1.{} Запустить сортировщик", GREEN, RESET);
    std::println("{}2.{} Режим автозапуска  [Сейчас: {}{}{}]", YELLOW, RESET, CYAN, autoStr, RESET);
    std::println("{}3.{} Папка сортировки   [Сейчас: {}{}{}]", YELLOW, RESET, CYAN, pathStr, RESET);
    std::println("{}0.{} Выход\n", RED, RESET);
    std::print("Выберите действие: ");
}

export void ShowAutoRunMenu(AppConfig& cfg) {
    ClearScreen();
    std::println("{}--- НАСТРОЙКА АВТОЗАПУСКА ---{}\n", BOLD, CYAN, RESET);
    std::println("1. Выключить");
    std::println("2. Включать при запуске");
    std::println("3. Запускать по таймеру (в часах)");
    std::println("0. Назад\n");
    std::print("Выбор: ");

    char choice = _getch();
    if (choice == '1') {
        cfg.autoRunMode = 0;
        ApplyAutoRunSettings(cfg);
        std::println("\n{}Автозапуск отключен.{}", GREEN, RESET);
        WaitAnyKey();
    }
    else if (choice == '2') {
        cfg.autoRunMode = 1;
        ApplyAutoRunSettings(cfg);
        std::println("\n{}Задача создана! Программа будет запускаться при входе в систему.{}", GREEN, RESET);
        WaitAnyKey();
    }
    else if (choice == '3') {
        ClearScreen();
        std::print("Введите интервал в часах (например, 2): ");
        cfg.customTimeHours = GetSafeInt();
        if (cfg.customTimeHours > 0) {
            cfg.autoRunMode = 2;
            ApplyAutoRunSettings(cfg);
            std::println("\n{}Задача создана! Программа будет запускаться каждые {} ч.{}", GREEN, cfg.customTimeHours, RESET);
        }
        else {
            std::println("\n{}[Ошибка] Время должно быть больше 0.{}", RED, RESET);
        }
        WaitAnyKey();
    }
    SaveConfig(cfg);
}

export void ShowFolderMenu(AppConfig& cfg) {
    ClearScreen();
    std::println("{}--- ВЫБОР ПАПКИ ---{}\n", BOLD, CYAN, RESET);
    std::println("1. Папка «Загрузки» по умолчанию");
    std::println("2. Указать свой путь");
    std::println("0. Назад\n");
    std::print("Выбор: ");

    char choice = _getch();
    if (choice == '1') {
        cfg.targetFolder = L"";
        SaveConfig(cfg);
    }
    else if (choice == '2') {
        ClearScreen();
        std::println("Введите полный путь к папке.\nПример: C:\\Users\\User\\Downloads\n");
        std::print("Путь: ");
        std::string input;
        std::getline(std::cin, input);

        int size = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, nullptr, 0);
        std::wstring wPath(size - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, wPath.data(), size);

        std::error_code ec;
        if (std::filesystem::is_directory(wPath, ec)) {
            cfg.targetFolder = wPath;
            SaveConfig(cfg);
            std::println("\n{}Путь сохранен!{}", GREEN, RESET);
        }
        else {
            std::println("\n{} [Ошибка] Папка не найдена!{}", RED, RESET);
        }
        WaitAnyKey();
    }
}