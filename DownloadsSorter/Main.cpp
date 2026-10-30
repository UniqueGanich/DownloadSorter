#include <Windows.h>

import std;
import Sorter;

int main() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    bool success = sorter();

    std::println("Нажмите любую кнопку чтобы выйти");
    std::cin.get();

    return success ? 0 : 1;
}