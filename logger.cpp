#include "logger.h"
#include <iostream>
#include <windows.h>

void SetColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void log(LogLevel level, const std::string& message) {
    // Make sure we start on a fresh line
    static bool needNewline = false;
    if (needNewline) {
        std::cout << std::endl;
        needNewline = false;
    }
    
    switch(level) {
        case 0:
            SetColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            std::cout << "[INFO] ";
            break;
        case 1:
            SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
            std::cout << "[ERROR] ";
            break;
        case 2:
            SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); // Yellow
            std::cout << "[WARN] ";
            break;
        case 3:
            SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            std::cout << "[SUCCESS] ";
            break;
        case 4:
            SetColor(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); // Cyan
            std::cout << "[PROCESS] ";
            break;
    }

    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset color
    std::cout << message << std::endl;
}

void printInitialBanner() {
    DWORD pid = GetCurrentProcessId();
    SetColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "=====================================\n";
    std::cout << "        Welcome to ThaiShell         \n";
    std::cout << "   Your professional command shell   \n";
    std::cout << "=====================================\n";
    SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << "Welcome to Tiny Shell!\n";
    std::cout << "Process ID of Tiny Shell: " << pid << "\n";
    std::cout << "Type 'help' to see available commands.\n";
}
