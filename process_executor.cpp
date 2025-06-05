#include "process_executor.h"
#include "process_manager.h"
#include "logger.h"
#include <string>
#include <windows.h>

using namespace std;

void ProcessExecutor::execute(const Command& cmd, ProcessManager& procManager) {
    string fullCmd = cmd.program;
    for (const auto& arg : cmd.args) {
        fullCmd += " " + arg;
    }    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    char* cmdLine = new char[fullCmd.length() + 1];
    strcpy(cmdLine, fullCmd.c_str());

    BOOL success = CreateProcessA(
        NULL,
        cmdLine,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE, // Tạo cửa sổ mới
        NULL,
        NULL,
        &si,
        &pi
    );

    if (success) {
        if (cmd.isBackground) {
            procManager.addProcess(pi.dwProcessId, fullCmd, pi.hProcess);
            log(INFO, "Started background process with PID: " + to_string(pi.dwProcessId) +
                      ", Command: " + fullCmd);
        } else {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    } else {
        log(LOG_ERROR, "Failed to start process: " + fullCmd);
    }

    delete[] cmdLine;
}
