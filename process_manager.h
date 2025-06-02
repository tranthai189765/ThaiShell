#pragma once
#include <vector>
#include <windows.h>
#include <string>
#include "command_parser.h"
struct BackgroundProcess {
    DWORD pid;
    std::string command;
    HANDLE handle;
    bool isRunning;
};

class ProcessManager {
public:
    static void addProcess(DWORD pid, const std::string& cmd, HANDLE handle);
    static void listProcesses();
    static void killProcess(DWORD pid);
    static void stopProcess(DWORD pid);
    static void resumeProcess(DWORD pid);
    bool handleCommand(const Command& cmd);
};
