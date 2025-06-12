#include "ctrl_c_handler.h"
#include "logger.h"
#include "process_manager.h"
#include <iostream>
#include <string>
#include <tlhelp32.h>
#include <vector>

// Initialize static members
HANDLE CtrlCHandler::currentForegroundProcess = NULL;
DWORD CtrlCHandler::currentForegroundProcessPid = 0;

void CtrlCHandler::initialize() {
    // Register the control handler
    if (!SetConsoleCtrlHandler(consoleCtrlHandler, TRUE)) {
        // Failed to set the handler
        log(LOG_ERROR, "Failed to set Ctrl+C handler: " + std::to_string(GetLastError()));
    } else {
        return;
    }
}

void CtrlCHandler::setForegroundProcess(HANDLE processHandle, DWORD pid) {
    currentForegroundProcess = processHandle;
    currentForegroundProcessPid = pid;
    log(INFO, "Set foreground process PID: " + std::to_string(pid));
}

void CtrlCHandler::resetForegroundProcess() {
    currentForegroundProcess = NULL;
    currentForegroundProcessPid = 0;
}

bool CtrlCHandler::hasForegroundProcess() {
    return currentForegroundProcess != NULL;
}

DWORD CtrlCHandler::getForegroundProcessPid() {
    return currentForegroundProcessPid;
}

BOOL WINAPI CtrlCHandler::consoleCtrlHandler(DWORD ctrlType) {
    // Only handle Ctrl+C and Ctrl+Break events
    if (ctrlType == CTRL_C_EVENT || ctrlType == CTRL_BREAK_EVENT) {
        // log(INFO, "Ctrl+C received");
        // If there's a foreground process running, terminate it
        if (currentForegroundProcess != NULL) {
            DWORD pid = currentForegroundProcessPid;
            
            // First find all child processes of the current foreground process
            std::vector<DWORD> childPids;
            
            // Take a snapshot of all processes
            HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (hProcessSnap != INVALID_HANDLE_VALUE) {
                PROCESSENTRY32 pe32;
                pe32.dwSize = sizeof(PROCESSENTRY32);
                
                // Iterate through all processes to find children
                if (Process32First(hProcessSnap, &pe32)) {
                    do {
                        // If this process's parent ID matches our target
                        if (pe32.th32ParentProcessID == pid) {
                            childPids.push_back(pe32.th32ProcessID);
                            log(INFO, "Found child process to terminate: PID " + 
                                std::to_string(pe32.th32ProcessID) + " (" + 
                                std::string(pe32.szExeFile) + ")");
                        }
                    } while (Process32Next(hProcessSnap, &pe32));
                }
                CloseHandle(hProcessSnap);
            }
            
            // Kill child processes first
            for (const auto& childPid : childPids) {
                log(INFO, "Terminating child process with PID: " + std::to_string(childPid));
                ProcessManager::killProcess(childPid);
            }
            
            // Then terminate the parent process
            // log(INFO, "Terminating child process with PID: " + std::to_string(pid));
            ProcessManager::killProcess(pid);
            
            // In any case, we should reset our handle
            resetForegroundProcess();
            
            // Print a new prompt line
            std::cout << std::endl;
        } else {
            // No foreground process, just print a message
            std::cout << "^C" << std::endl;
            log(INFO, "Ctrl+C pressed (no foreground process to terminate)");
        }
        
        // ALWAYS return TRUE for Ctrl+C events to prevent Windows from terminating our shell
        // This is critical to avoid the "Terminate batch job (Y/N)" prompt
        return TRUE;
    }
    
    // Return FALSE for other events to let Windows handle them
    return FALSE;
}