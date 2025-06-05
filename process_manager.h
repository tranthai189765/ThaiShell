#pragma once

// Prevent Windows headers from defining conflicting 'byte' type
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WINDOWS_LEAN_AND_MEAN
#define WINDOWS_LEAN_AND_MEAN
#endif

#include <vector>
#include <windows.h>
#include <string>
#include "command_parser.h"
#include "history.h"
#include "ctrl_c_handler.h"
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
    static void findChildProcesses(DWORD parentPid);
    static void resumeProcess(DWORD pid);
    bool handleCommand(const Command& cmd);
};
