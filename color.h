#ifndef COLOR_H
#define COLOR_H

#ifdef _WIN32
#include <windows.h>
#endif

// ANSI color codes
#define COLOR_GREEN "\033[32m"
#define COLOR_CYAN "\033[36m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RESET "\033[0m"

inline void enableANSISupport() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

#endif // COLOR_H
