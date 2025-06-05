#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <windows.h>

// Structure to store process history information
struct ProcessHistoryEntry {
    DWORD pid;
    std::string processName;
    std::time_t startTime;
    std::time_t endTime; // -1 if still running
};

class ProcessHistory {
public:
    // Add a new process to history
    static void addProcess(DWORD pid, const std::string& processName);
    
    // Update the end time for a process that has terminated
    static void updateEndTime(DWORD pid);
    
    // Display the process history
    static void displayHistory();
    
    // Format the timestamp into a readable string
    static std::string formatTimestamp(std::time_t timestamp);

private:
    static std::vector<ProcessHistoryEntry> processHistory;
};

// Utility function to get current time
std::time_t getCurrentTime();