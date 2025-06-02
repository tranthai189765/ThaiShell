#include "process_manager.h"
#include "logger.h"
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>

using namespace std;

vector<BackgroundProcess> bgProcesses;

void ProcessManager::addProcess(DWORD pid, const string& cmd, HANDLE handle) {
    bgProcesses.push_back({pid, cmd, handle, true});
    log(PROCESS, "Added process with PID: " + to_string(pid) + " | Command: " + cmd);
}

void ProcessManager::listProcesses() {
    log(INFO, "Listing background processes:");
    for (const auto& proc : bgProcesses) {
        string status = proc.isRunning ? "Running" : "Stopped";
        log(PROCESS, "  PID: " + to_string(proc.pid) +
                     " | Command: " + proc.command +
                     " | Status: " + status);
    }
}

void ProcessManager::killProcess(DWORD pid) {
    for (auto it = bgProcesses.begin(); it != bgProcesses.end(); ++it) {
        if (it->pid == pid) {
            if (TerminateProcess(it->handle, 0)) {
                WaitForSingleObject(it->handle, INFINITE);
                CloseHandle(it->handle);
                log(SUCCESS, "Killed process with PID: " + to_string(pid));
                bgProcesses.erase(it);
            } else {
                log(LOG_ERROR, "Failed to kill process with PID: " + to_string(pid));
            }
            return;
        }
    }
    log(WARN, "No background process found with PID: " + to_string(pid));
}

void ProcessManager::stopProcess(DWORD pid) {
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        log(LOG_ERROR, "Failed to take thread snapshot.");
        return;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hThreadSnap, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pid) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (hThread) {
                    SuspendThread(hThread);
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hThreadSnap, &te32));
    }
    CloseHandle(hThreadSnap);

    for (auto& proc : bgProcesses) {
        if (proc.pid == pid) {
            proc.isRunning = false;
            log(INFO, "Stopped process with PID: " + to_string(pid));
            return;
        }
    }
    log(WARN, "No background process found with PID: " + to_string(pid));
}

void ProcessManager::resumeProcess(DWORD pid) {
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        log(LOG_ERROR, "Failed to take thread snapshot.");
        return;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hThreadSnap, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pid) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (hThread) {
                    ResumeThread(hThread);
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hThreadSnap, &te32));
    }
    CloseHandle(hThreadSnap);

    for (auto& proc : bgProcesses) {
        if (proc.pid == pid) {
            proc.isRunning = true;
            log(INFO, "Resumed process with PID: " + to_string(pid));
            return;
        }
    }
    log(WARN, "No background process found with PID: " + to_string(pid));
}

bool ProcessManager::handleCommand(const Command& cmd) {
    if (cmd.program == "list") {
        listProcesses();
        return true;
    }
    if (cmd.program == "kill" && !cmd.args.empty()) {
        killProcess(stoi(cmd.args[0]));
        return true;
    }
    if (cmd.program == "stop" && !cmd.args.empty()) {
        stopProcess(stoi(cmd.args[0]));
        return true;
    }
    if (cmd.program == "resume" && !cmd.args.empty()) {
        resumeProcess(stoi(cmd.args[0]));
        return true;
    }
    return false;
}
