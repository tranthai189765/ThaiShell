#include "process_executor.h"
#include "process_manager.h"
#include "logger.h"
#include "ctrl_c_handler.h"
#include <string>
#include <windows.h>
#include <iostream>

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
                      ", Command: " + fullCmd);        } else {
            // Register the foreground process with the Ctrl+C handler
            CtrlCHandler::setForegroundProcess(pi.hProcess, pi.dwProcessId);
            
            // Wait for the process to finish or be terminated
            DWORD waitResult = WaitForSingleObject(pi.hProcess, INFINITE);
            
            // Get the exit code to check if the process was terminated by Ctrl+C
            DWORD exitCode = 0;
            GetExitCodeProcess(pi.hProcess, &exitCode);
            
            // Log the exit if it was terminated (not normal exit)
            if (exitCode != 0) {
                log(INFO, "Process exited with code: " + to_string(exitCode));
            }
            
            // Reset the Ctrl+C handler once the process is done
            CtrlCHandler::resetForegroundProcess();
            
            // Close the process and thread handles
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            
            // Print a new prompt line after the process exits
            cout << endl;
        }
    } else {
        log(LOG_ERROR, "Failed to start process: " + fullCmd);
    }

    delete[] cmdLine;
}
