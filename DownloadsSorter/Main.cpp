#include <Windows.h>

import std;
import Sorter;

int main() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    bool success = false;
    try {
        success = sorter();
    }
    catch (const std::exception& e) {
        std::println(std::cerr, "\033[31m[Критическая ошибка] {}\033[0m", e.what());
    }
    catch (...) {
        std::println(std::cerr, "\033[31m[Критическая ошибка] Неизвестное исключение.\033[0m");
    }

    std::println("Нажмите любую кнопку чтобы выйти");
    std::cin.get();

    return success ? 0 : 1;
}