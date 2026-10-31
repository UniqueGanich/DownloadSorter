#pragma once
#include <string>

inline const std::string RESET = "\033[0m";
inline const std::string RED = "\033[31m";
inline const std::string GREEN = "\033[32m";
inline const std::string YELLOW = "\033[33m";
inline const std::string CYAN = "\033[36m";
inline const std::string BOLD = "\033[1m";

void InitConsole();