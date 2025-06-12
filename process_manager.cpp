#include "process_manager.h"
#include "logger.h"
#include <tlhelp32.h>
#include <string>
#include <vector>
#include<iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

vector<BackgroundProcess> bgProcesses;
ProcessManager processManager;

void ProcessManager::addProcess(DWORD pid, const string& cmd, HANDLE handle) {
    bgProcesses.push_back({pid, cmd, handle, true});
    log(PROCESS, "Added process with PID: " + to_string(pid) + " | Command: " + cmd);
}

// void startProcess(const string& cmd, bool isBackground) {
//     STARTUPINFOA si = { sizeof(si) };
//     PROCESS_INFORMATION pi;

//     char* cmdLine = new char[cmd.length() + 1];
//     strcpy(cmdLine, cmd.c_str());

//     BOOL success = CreateProcessA(
//         NULL,
//         cmdLine,
//         NULL,
//         NULL,
//         FALSE,
//         isBackground ? CREATE_NEW_CONSOLE : 0, // Tạo cửa sổ mới nếu là background
//         NULL,
//         NULL,
//         &si,
//         &pi
//     );    if (success) {
//         if (isBackground) {
//             // Store the child process ID (PID) and handle
//             ProcessManager::addProcess(pi.dwProcessId, cmd, pi.hProcess);
//             log(INFO, "Started background process with PID: " + to_string(pi.dwProcessId) +
//                       ", Command: " + cmd);
//         } else {
//             WaitForSingleObject(pi.hProcess, INFINITE);
//             CloseHandle(pi.hProcess);
//             CloseHandle(pi.hThread);
//         }
//     } else {
//         log(LOG_ERROR, "Failed to start process: " + cmd);
//     }

//     delete[] cmdLine;
// }

void startProcess(const string& cmd, bool isBackground) {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    char* cmdLine = new char[cmd.length() + 1];
    strcpy(cmdLine, cmd.c_str());

    BOOL success = CreateProcessA(
        NULL,
        cmdLine,
        NULL,
        NULL,
        FALSE,
        isBackground ? CREATE_NEW_CONSOLE : 0, // Create new console if background
        NULL,
        NULL,
        &si,
        &pi
    );    
    
    if (success) {        if (isBackground) {
            // Store the original PID first
            DWORD originalPid = pi.dwProcessId;
            
            // Check if this is likely to be a GUI application
            bool isGuiApp = false;
            string lowerCmd = cmd;
            transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), ::tolower);
            
            // Enhanced heuristic: check if the command contains typical GUI apps
            if (lowerCmd.find("notepad") != string::npos || 
                lowerCmd.find("explorer") != string::npos || 
                lowerCmd.find("mspaint") != string::npos ||
                lowerCmd.find("calc") != string::npos ||
                lowerCmd.find("winword") != string::npos ||
                lowerCmd.find("excel") != string::npos ||
                lowerCmd.find("chrome") != string::npos ||
                lowerCmd.find("firefox") != string::npos ||
                lowerCmd.find("msedge") != string::npos) {
                isGuiApp = true;
            }
            
            // Add the original process to our tracking list, but not to history yet
            ProcessManager::addProcess(pi.dwProcessId, cmd, pi.hProcess);
            
            log(INFO, "Started background process with PID: " + to_string(pi.dwProcessId) +
                      ", Command: " + cmd);
            
            // Give the process a moment to create child processes 
            Sleep(500);
            
            bool childProcessFound = false;
            
            // Now check if this process spawned any children
            HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (hSnapshot != INVALID_HANDLE_VALUE) {
                PROCESSENTRY32 pe32;
                pe32.dwSize = sizeof(PROCESSENTRY32);
                if (Process32First(hSnapshot, &pe32)) {
                    do {
                        // If we find a process whose parent is our launched process
                        if (pe32.th32ParentProcessID == pi.dwProcessId) {
                            // Open this child process
                            HANDLE childHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                            if (childHandle) {
                                // Get the name of the executable
                                char exeName[MAX_PATH];
                                DWORD size = MAX_PATH;
                                if (QueryFullProcessImageNameA(childHandle, 0, exeName, &size)) {
                                    // Extract just the filename from the path
                                    string fullPath = exeName;
                                    size_t lastSlash = fullPath.find_last_of("\\");
                                    string childName = fullPath.substr(lastSlash + 1);
                                    
                                    // Add the child to our process list
                                    ProcessManager::addProcess(pe32.th32ProcessID, childName, childHandle);
                                    
                                    // For GUI apps, only add the child process to history
                                    // For non-GUI apps, add both parent and child
                                    if (isGuiApp) {
                                        // Only add the child to history (not the parent)
                                        ProcessHistory::addProcess(pe32.th32ProcessID, childName);
                                        childProcessFound = true;
                                    } else {
                                        // Add both parent and child to history
                                        if (!childProcessFound) {
                                            ProcessHistory::addProcess(originalPid, cmd);
                                        }
                                        ProcessHistory::addProcess(pe32.th32ProcessID, childName);
                                    }
                                    
                                    log(INFO, "Tracked child process with PID: " + to_string(pe32.th32ProcessID) +
                                        ", Command: " + childName + " (child of PID: " + to_string(pi.dwProcessId) + ")");
                                } else {
                                    CloseHandle(childHandle); // Close handle if we couldn't get the name
                                }
                            }
                        }
                    } while (Process32Next(hSnapshot, &pe32));
                }
                CloseHandle(hSnapshot);
            }
            
            // If we didn't find any child processes, add the parent to history
            if (!childProcessFound) {
                ProcessHistory::addProcess(originalPid, cmd);
            }        } else {
            // Check if this is likely to be a GUI application
            bool isGuiApp = false;
            string lowerCmd = cmd;
            transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), ::tolower);
            
            // Enhanced heuristic: check if the command contains typical GUI apps
            if (lowerCmd.find("notepad") != string::npos || 
                lowerCmd.find("explorer") != string::npos || 
                lowerCmd.find("mspaint") != string::npos ||
                lowerCmd.find("calc") != string::npos ||
                lowerCmd.find("winword") != string::npos ||
                lowerCmd.find("excel") != string::npos ||
                lowerCmd.find("chrome") != string::npos ||
                lowerCmd.find("firefox") != string::npos ||
                lowerCmd.find("msedge") != string::npos ||
                lowerCmd.find(".exe") != string::npos) {
                isGuiApp = true;
            }
            
            log(INFO, "Running " + cmd + " in foreground mode");
              if (isGuiApp) {
                log(INFO, "GUI application detected. Will track child processes...");
                // Wait a little for child processes to spawn
                Sleep(500);
                
                // Store the original PID first
                DWORD originalPid = pi.dwProcessId;
                
                // Keep track of all child PIDs
                vector<DWORD> childPids;
                
                // Now check if this process spawned any children
                HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                if (hSnapshot != INVALID_HANDLE_VALUE) {
                    PROCESSENTRY32 pe32;
                    pe32.dwSize = sizeof(PROCESSENTRY32);
                    if (Process32First(hSnapshot, &pe32)) {
                        do {
                            // If we find a process whose parent is our launched process
                            if (pe32.th32ParentProcessID == originalPid) {
                                childPids.push_back(pe32.th32ProcessID);
                                log(INFO, "Found child process PID: " + to_string(pe32.th32ProcessID));
                            }
                        } while (Process32Next(hSnapshot, &pe32));
                    }
                    CloseHandle(hSnapshot);
                }
                
                // For foreground applications, we need to wait for either:
                // 1. The parent process to finish if it's the main app
                // 2. OR wait for all child processes if the parent is just a launcher
                
                log(INFO, "Waiting for the application to complete...");
                bool waitForChildren = false;
                
                // First check if parent process is just a quick launcher
                DWORD waitResult = WaitForSingleObject(pi.hProcess, 2000); // Wait up to 2 seconds for parent
                
                if (waitResult == WAIT_OBJECT_0 && !childPids.empty()) {
                    // Parent exited quickly and we found child processes - this means the parent was just a launcher
                    log(INFO, "Parent process exited, waiting for child processes to finish");
                    waitForChildren = true;                } else if (waitResult == WAIT_TIMEOUT) {                    // Parent is still running after timeout - this means the parent is the main application
                    log(INFO, "Parent process is the main application, waiting for it to finish");
                    
                    // Add the parent process to history since it's the main application
                    ProcessHistory::addProcess(pi.dwProcessId, cmd);
                    
                    // Register the parent process with the Ctrl+C handler
                    CtrlCHandler::setForegroundProcess(pi.hProcess, pi.dwProcessId);
                    log(INFO, "Registered parent process PID " + to_string(pi.dwProcessId) + " with Ctrl+C handler");
                    
                    // Wait for it to finish
                    WaitForSingleObject(pi.hProcess, INFINITE);
                    
                    // Reset the Ctrl+C handler once finished
                    CtrlCHandler::resetForegroundProcess();
                    
                    // Update the end time since it has finished
                    ProcessHistory::updateEndTime(pi.dwProcessId);
                }
                
                // If the parent was just a launcher, wait for the child processes
                if (waitForChildren) {
                    vector<HANDLE> childHandles;
                    
                    // Open handles to all child processes
                    for (const DWORD& childPid : childPids) {
                        HANDLE childHandle = OpenProcess(SYNCHRONIZE, FALSE, childPid);
                        if (childHandle) {
                            childHandles.push_back(childHandle);
                            
                            // Get the name of the process for history
                            HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, childPid);
                            if (processHandle) {
                                char exeName[MAX_PATH];
                                DWORD size = MAX_PATH;
                                string childName = "Unknown";
                                
                                if (QueryFullProcessImageNameA(processHandle, 0, exeName, &size)) {
                                    string fullPath = exeName;
                                    size_t lastSlash = fullPath.find_last_of("\\");
                                    childName = fullPath.substr(lastSlash + 1);
                                }
                                
                                // Add only the child process to history
                                ProcessHistory::addProcess(childPid, childName);
                                
                                CloseHandle(processHandle);
                            }
                        }
                    }
                      if (!childHandles.empty()) {
                        log(INFO, "Waiting for all " + to_string(childHandles.size()) + " child processes to finish...");
                        
                        // Register the first child process with the Ctrl+C handler
                        if (childHandles.size() > 0) {
                            HANDLE primaryChild = childHandles[0];
                            DWORD primaryChildPid = childPids[0];
                            
                            // Register this child process with the Ctrl+C handler
                            CtrlCHandler::setForegroundProcess(primaryChild, primaryChildPid);
                        }
                        
                        // Wait for all child processes to finish (foreground mode)
                        WaitForMultipleObjects(childHandles.size(), childHandles.data(), TRUE, INFINITE);
                        
                        // Reset the Ctrl+C handler once finished
                        CtrlCHandler::resetForegroundProcess();
                        
                        // Close all the child handles
                        for (HANDLE h : childHandles) {
                            CloseHandle(h);
                        }
                        
                        log(INFO, "All child processes have exited");
                        
                        // Update end time for all child processes in history
                        for (const DWORD& childPid : childPids) {
                            ProcessHistory::updateEndTime(childPid);
                        }
                    }
                }
                
                // Close original process handles
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                
                // Update end time for the original process
                ProcessHistory::updateEndTime(originalPid);
                
                log(INFO, "Process completed");
            } else {                // For standard console applications, add to history and wait for them to finish
                log(INFO, "Waiting for process to complete...");
                
                // For console apps, add to history here
                ProcessHistory::addProcess(pi.dwProcessId, cmd);
                
                // Register the console process with the Ctrl+C handler
                CtrlCHandler::setForegroundProcess(pi.hProcess, pi.dwProcessId);
                log(INFO, "Registered console process PID " + to_string(pi.dwProcessId) + " with Ctrl+C handler");
                
                WaitForSingleObject(pi.hProcess, INFINITE);
                
                // Reset the Ctrl+C handler once finished
                CtrlCHandler::resetForegroundProcess();
                
                // Update end time since the process has finished
                ProcessHistory::updateEndTime(pi.dwProcessId);
                
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                
                log(INFO, "Process completed");
            }
        }
    } else {
        log(LOG_ERROR, "Failed to start process: " + cmd);
    }

    delete[] cmdLine;
}

void ProcessManager::listProcesses() {
    log(INFO, "Listing background processes:");
    
    // Print header with consistent order of columns
    cout << setw(10) << left << "PID" 
         << setw(40) << left << "Command" 
         << setw(20) << left << "Parent PID (PPID)"
         << setw(20) << left << "Status" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    
    // Get ThaiShell's own PID
    DWORD currentPID = GetCurrentProcessId();
    
    // Keep track of PIDs we've already displayed to avoid duplicates
    vector<DWORD> displayedPids;
      
    for (const auto& proc : bgProcesses) {
        // First check if this process still exists or has been replaced by a child
        DWORD actualPid = proc.pid;
        string actualCommand = proc.command;
        bool foundActualProcess = false;
        
        // Take a snapshot to find the actual running process
        HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(hProcessSnap, &pe32)) {
                do {
                    // Check if this is our process or a child of our process
                    if (pe32.th32ProcessID == proc.pid) {
                        foundActualProcess = true;
                        actualCommand = pe32.szExeFile;
                        break;
                    }
                    
                    // Try to identify child processes related to our command
                    if (pe32.th32ParentProcessID == proc.pid) {
                        // This is potentially the real process we want to show
                        // For example, when starting "calc.exe", Windows might create a launcher
                        // process first, which then starts the actual calculator process
                        char exeName[MAX_PATH];
                        HANDLE childProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
                        if (childProc) {
                            DWORD size = MAX_PATH;
                            if (QueryFullProcessImageNameA(childProc, 0, exeName, &size)) {
                                string fullPath = exeName;
                                size_t lastSlash = fullPath.find_last_of("\\");
                                string childName = fullPath.substr(lastSlash + 1);
                                
                                // If the executable name matches what we're looking for
                                if (childName.find(proc.command) != string::npos || 
                                    proc.command.find(childName) != string::npos) {
                                    actualPid = pe32.th32ProcessID;
                                    actualCommand = childName;
                                    foundActualProcess = true;
                                    CloseHandle(childProc);
                                    break;
                                }
                            }
                            CloseHandle(childProc);
                        }
                    }
                } while (Process32Next(hProcessSnap, &pe32));
            }
            CloseHandle(hProcessSnap);
        }        // Get the parent process ID (PPID) for this process
        DWORD ppid = currentPID; // Default to ThaiShell's PID
        // Try to get actual PPID from process snapshot
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(hSnapshot, &pe32)) {
                do {
                    if (pe32.th32ProcessID == actualPid) {
                        ppid = pe32.th32ParentProcessID;
                        break;
                    }
                } while (Process32Next(hSnapshot, &pe32));
            }
            CloseHandle(hSnapshot);
        }
        
        // Check real process status
        string status = "Unknown";
        
        // If we're using a different PID than originally stored, get a handle to it
        HANDLE processHandle = proc.handle;
        if (actualPid != proc.pid) {
            processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, actualPid);
        }
        
        if (processHandle) {
            DWORD exitCode;
            if (GetExitCodeProcess(processHandle, &exitCode)) {
                if (exitCode == STILL_ACTIVE) {
                    // Check if the process is actually running by checking its threads
                    bool isRunning = false;
                    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
                    if (hThreadSnap != INVALID_HANDLE_VALUE) {
                        THREADENTRY32 te32;
                        te32.dwSize = sizeof(THREADENTRY32);
                        if (Thread32First(hThreadSnap, &te32)) {
                            do {
                                if (te32.th32OwnerProcessID == actualPid) {
                                    isRunning = true;  // If we find threads for this process, it exists
                                    break;
                                }
                            } while (Thread32Next(hThreadSnap, &te32));
                        }
                        CloseHandle(hThreadSnap);
                    }
                      // If the process exists, check if threads are actually running or suspended
                    if (isRunning) {
                        // For processes we track, we need to check if the threads are actually suspended
                        bool actuallyRunning = false;
                        HANDLE hThreadSnap2 = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
                        if (hThreadSnap2 != INVALID_HANDLE_VALUE) {
                            THREADENTRY32 te32;
                            te32.dwSize = sizeof(THREADENTRY32);
                            if (Thread32First(hThreadSnap2, &te32)) {
                                do {                                if (te32.th32OwnerProcessID == actualPid) {
                                        HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                                        if (hThread) {
                                            // Try to suspend the thread to check its current state
                                            DWORD suspendCount = SuspendThread(hThread);
                                            if (suspendCount != (DWORD)-1) {
                                                // If returned 0, thread was running
                                                if (suspendCount == 0) {
                                                    actuallyRunning = true;
                                                }
                                                // Always restore original state
                                                ResumeThread(hThread);
                                            }
                                            CloseHandle(hThread);
                                            if (actuallyRunning) break;  // Found at least one running thread
                                        }
                                    }
                                } while (Thread32Next(hThreadSnap2, &te32));
                            }
                            CloseHandle(hThreadSnap2);
                        }
                        
                        // Update status based on actual thread state, not just our internal flag
                        if (actuallyRunning) {
                            status = "Running";
                        } else {
                            status = "Suspended";
                        }
                    } else {
                        status = "Zombie Process";  // Process exists but has no threads
                    }
                } else {
                    status = "Terminated (code: " + to_string(exitCode) + ")";
                }
            } else {
                status = "Error checking";
            }
        } else {
            status = proc.isRunning ? "Running" : "Stopped";
        }            // Check if we've already displayed this PID
        if (find(displayedPids.begin(), displayedPids.end(), actualPid) != displayedPids.end()) {
            continue; // Skip displaying this process if PID has already been shown
        }
        
        // Output to console with formatting
        cout << setw(10) << left << actualPid  // Use the actual PID we found
             << setw(40) << left << actualCommand  // Use the actual command/process name
             << setw(20) << left << ppid
             << setw(20) << left << status << endl;
             
        // Add this PID to our displayed list to avoid duplicates
        displayedPids.push_back(actualPid);
    }
}

void ListProcesses() {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        cout << "Failed to create process snapshot." << endl;
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
        cout << "Failed to retrieve process information." << endl;
        CloseHandle(hProcessSnap);
        return;
    }    // Print process information with clear PID vs PPID labels
    cout << setw(10) << left << "PID"
         << setw(40) << left << "Process Name"
         << setw(20) << left << "Parent PID (PPID)" 
         << setw(20) << left << "Status" << endl;
    cout << "-----------------------------------------------------------------------------------------------------" << endl;

    do {        // Get process status
        string processStatus = "Unknown";
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
        if (hProcess) {
            DWORD exitCode;
            if (GetExitCodeProcess(hProcess, &exitCode)) {
                if (exitCode == STILL_ACTIVE) {
                    // Better way to check process status - first check if it exists
                    bool hasThreads = false;
                    bool allThreadsSuspended = true;
                    
                    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
                    if (hThreadSnap != INVALID_HANDLE_VALUE) {
                        THREADENTRY32 te32;
                        te32.dwSize = sizeof(THREADENTRY32);
                        if (Thread32First(hThreadSnap, &te32)) {
                            do {
                                if (te32.th32OwnerProcessID == pe32.th32ProcessID) {
                                    hasThreads = true;
                                    
                                    // Check if this thread is suspended without actually suspending it
                                    HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
                                    if (hThread) {                                        // Better way to check if a thread is suspended:
                                        // 1. Try to suspend it
                                        // 2. If successful, check the suspension count
                                        // 3. Always resume it to restore original state
                                        DWORD suspendCount = SuspendThread(hThread);
                                        if (suspendCount != (DWORD)-1) {
                                            // If suspension count > 0, thread was already suspended before our call
                                            if (suspendCount == 0) {
                                                allThreadsSuspended = false; // Thread was not suspended
                                            }
                                            // Resume the thread to restore its state
                                            ResumeThread(hThread);
                                        } else {
                                            // If we can't suspend, assume thread is running
                                            allThreadsSuspended = false;
                                        }
                                        CloseHandle(hThread);
                                        
                                        if (!allThreadsSuspended) break;
                                    }
                                }
                            } while (Thread32Next(hThreadSnap, &te32));
                        }
                        CloseHandle(hThreadSnap);
                    }
                    
                if (!hasThreads) {
                        processStatus = "No Threads";
                    } else if (allThreadsSuspended) {
                        processStatus = "Suspended";
                    } else {
                        processStatus = "Running";
                    }
                } else {
                    processStatus = "Terminated (" + to_string(exitCode) + ")";
                }
            }
            CloseHandle(hProcess);
        } else {
            DWORD error = GetLastError();
            if (error == ERROR_ACCESS_DENIED) {
                processStatus = "Access Denied";
            }
        }
        
        cout << setw(10) << left << pe32.th32ProcessID
             << setw(40) << left << pe32.szExeFile
             << setw(20) << left << pe32.th32ParentProcessID
             << setw(20) << left << processStatus << endl;
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
}

void ProcessManager::killProcess(DWORD pid) {
    // First try to find the process in our background processes list
    for (auto it = bgProcesses.begin(); it != bgProcesses.end(); ++it) {
        if (it->pid == pid) {
            if (TerminateProcess(it->handle, 0)) {
                WaitForSingleObject(it->handle, INFINITE);
                CloseHandle(it->handle);                // Update the end time in process history
                ProcessHistory::updateEndTime(pid);
                
                log(INFO, "Process with PID: " + to_string(pid) + " terminated successfully");
                // cout << "Process with PID: " << pid << " terminated successfully" << endl;
                bgProcesses.erase(it);
            } else {
                DWORD error = GetLastError();
                // If access denied, try again with more privileges
                if (error == 5) { // ERROR_ACCESS_DENIED
                    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
                    if (hProcess != NULL) {
                        if (TerminateProcess(hProcess, 0)) {                            WaitForSingleObject(hProcess, INFINITE);
                            CloseHandle(hProcess);
                            log(INFO, "Process with PID: " + to_string(pid) + " terminated successfully");
                            // cout << "Process with PID: " << pid << " terminated successfully" << endl;
                            bgProcesses.erase(it);
                            return;
                        }
                        CloseHandle(hProcess);
                    }
                    log(WARN, "Cannot kill process with PID: " + to_string(pid) + 
                              " - This may require running ThaiShell as Administrator");
                } else {
                    log(LOG_ERROR, "Failed to kill process with PID: " + to_string(pid) + 
                                  ", error code: " + to_string(error));
                }
            }
            return;
        }
    }
      // If not found in our list, try to kill it as an external process
    // Try with more privileges (PROCESS_ALL_ACCESS) to handle protected processes
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess != NULL) {
        if (TerminateProcess(hProcess, 0)) {            WaitForSingleObject(hProcess, INFINITE);
            CloseHandle(hProcess);              // Update the end time in process history for external processes too
            ProcessHistory::updateEndTime(pid);
            
            // log(INFO, "Process with PID: " + to_string(pid) + " terminated successfully");
            cout << "Process with PID: " << pid << " terminated successfully" << endl;
        } else {
            // If failed, check if it's a Windows system process
            DWORD error = GetLastError();
            if (error == 5) { // ERROR_ACCESS_DENIED
                log(WARN, "Cannot kill process with PID: " + to_string(pid) + 
                          " - This may be a system process or requires elevated privileges");
            } else {
                log(LOG_ERROR, "Failed to kill external process with PID: " + to_string(pid) + 
                              ", error code: " + to_string(error));
            }
        }
    } else {
        DWORD error = GetLastError();
        if (error == 5) { // ERROR_ACCESS_DENIED
            log(WARN, "Cannot access process with PID: " + to_string(pid) + 
                      " - This may require running ThaiShell as Administrator");
        } else {
            log(WARN, "No process found with PID: " + to_string(pid) + 
                      ", error code: " + to_string(error));
        }
    }
}

void ProcessManager::stopProcess(DWORD pid) {
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        log(LOG_ERROR, "Failed to take thread snapshot.");
        return;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);
    
    bool threadsFound = false;
    int suspendedThreads = 0;
    int totalThreads = 0;
    
    // First attempt to suspend all threads in the process
    if (Thread32First(hThreadSnap, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pid) {
                totalThreads++;
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (hThread) {
                    // Try to suspend thread and check result
                    DWORD suspendCount = SuspendThread(hThread);
                    if (suspendCount != (DWORD)-1) {
                        suspendedThreads++;
                        threadsFound = true;
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hThreadSnap, &te32));
    }
    CloseHandle(hThreadSnap);
    
    // Then update our tracking record
    for (auto& proc : bgProcesses) {
        if (proc.pid == pid) {
            proc.isRunning = false;
            if (suspendedThreads > 0) {
                log(INFO, "Stopped process with PID: " + to_string(pid) + 
                    " (" + to_string(suspendedThreads) + "/" + to_string(totalThreads) + " threads suspended)");
            } else {
                log(WARN, "Failed to suspend any threads for process with PID: " + to_string(pid));
            }
            return;
        }
    }
    
    // Even if the process isn't in our list, let the user know if we actually stopped threads
    if (threadsFound) {
        log(INFO, "Stopped external process with PID: " + to_string(pid) + 
            " (" + to_string(suspendedThreads) + "/" + to_string(totalThreads) + " threads suspended)");
    } else {
        log(WARN, "No process found with PID: " + to_string(pid));
    }
}

void ProcessManager::resumeProcess(DWORD pid) {
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        log(LOG_ERROR, "Failed to take thread snapshot.");
        return;
    }

    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);
    
    bool threadsFound = false;

    if (Thread32First(hThreadSnap, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pid) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                if (hThread) {
                    ResumeThread(hThread);
                    CloseHandle(hThread);
                    threadsFound = true;
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
    
    // Even if the process isn't in our list, let the user know if we actually resumed threads
    if (threadsFound) {
        log(INFO, "Resumed external process with PID: " + to_string(pid) + " (not in background process list)");
    } else {
        log(WARN, "No process found with PID: " + to_string(pid));
    }
}

void ProcessManager::findChildProcesses(DWORD parentPid) {
    // First check if the parent process exists
    HANDLE hParentProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, parentPid);
    if (!hParentProcess) {
        log(WARN, "No process found with PID: " + std::to_string(parentPid));
        return;
    }
    CloseHandle(hParentProcess);

    // Take a snapshot of all processes
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        log(LOG_ERROR, "Failed to take process snapshot.");
        return;
    }

    // Initialize process entry structure
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Try to get the first process
    if (!Process32First(hProcessSnap, &pe32)) {
        log(LOG_ERROR, "Failed to retrieve process information.");
        CloseHandle(hProcessSnap);
        return;
    }

    // Prepare for displaying the results
    std::vector<std::pair<DWORD, std::string>> childProcesses;
    
    // Loop through all processes to find children
    do {
        // Check if this process's parent ID matches our target
        if (pe32.th32ParentProcessID == parentPid) {
            childProcesses.push_back({pe32.th32ProcessID, std::string(pe32.szExeFile)});
        }
    } while (Process32Next(hProcessSnap, &pe32));
    
    CloseHandle(hProcessSnap);

    // Print the results
    log(INFO, "Child processes of PID " + std::to_string(parentPid) + ":");
    
    if (childProcesses.empty()) {
        log(INFO, "No child processes found.");
    } else {
        // Print a formatted header
        std::cout << std::setw(10) << std::left << "PID" 
                 << std::setw(40) << std::left << "Process Name" << std::endl;
        std::cout << "---------------------------------------------" << std::endl;
        
        // Print each child process
        for (const auto& child : childProcesses) {
            std::cout << std::setw(10) << std::left << child.first 
                     << std::setw(40) << std::left << child.second << std::endl;
        }
        
        log(INFO, "Found " + std::to_string(childProcesses.size()) + " child process(es).");
    }
}

bool ProcessManager::handleCommand(const Command& cmd) {
    if (cmd.program == "globalList") {
        ListProcesses();
        return true;
    }
    if (cmd.program == "myList") {
        listProcesses();
        return true;
    }
    if (cmd.program == "start" && !cmd.args.empty()) {
        string fullCmd = cmd.args[0];
        for (size_t i = 1; i < cmd.args.size(); ++i) {
            fullCmd += " " + cmd.args[i];
        }
        startProcess(fullCmd, cmd.isBackground);
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
    if (cmd.program == "history") {
        ProcessHistory::displayHistory();
        return true;
    }
    if (cmd.program == "child" && !cmd.args.empty()) {
        findChildProcesses(stoi(cmd.args[0]));
        return true;
    }
    return false;
}
