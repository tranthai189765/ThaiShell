#ifndef CTRL_C_HANDLER_H
#define CTRL_C_HANDLER_H

#include <windows.h>

// Class to handle Ctrl+C events in the shell
class CtrlCHandler {
public:
    // Initialize the Ctrl+C handler
    static void initialize();
    
    // Set the current foreground process handle and PID
    static void setForegroundProcess(HANDLE processHandle, DWORD pid);
    
    // Reset the foreground process tracking
    static void resetForegroundProcess();
    
    // Get if there's a foreground process running
    static bool hasForegroundProcess();
    
    // Get the foreground process PID
    static DWORD getForegroundProcessPid();

private:
    // Actual Windows console control handler callback
    static BOOL WINAPI consoleCtrlHandler(DWORD ctrlType);
    
    // Process handle for the currently running foreground process
    static HANDLE currentForegroundProcess;
    
    // Process ID of the currently running foreground process
    static DWORD currentForegroundProcessPid;
};

#endif // CTRL_C_HANDLER_H