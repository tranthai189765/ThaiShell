#include "history.h"
#include "logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

// Initialize the static member
vector<ProcessHistoryEntry> ProcessHistory::processHistory;

// Get the current time as a timestamp
time_t getCurrentTime() {
    return chrono::system_clock::to_time_t(chrono::system_clock::now());
}

// Add a new process to history
void ProcessHistory::addProcess(DWORD pid, const string& processName) {
    ProcessHistoryEntry entry;
    entry.pid = pid;
    entry.processName = processName;
    entry.startTime = getCurrentTime();
    entry.endTime = -1; // -1 indicates it's still running
    
    processHistory.push_back(entry);
}

// Update the end time for a process that has terminated
void ProcessHistory::updateEndTime(DWORD pid) {
    for (auto& entry : processHistory) {
        if (entry.pid == pid && entry.endTime == -1) {
            entry.endTime = getCurrentTime();
            return;
        }
    }
}

// Format timestamp into a readable string
string ProcessHistory::formatTimestamp(time_t timestamp) {
    if (timestamp == -1) {
        return "Running";
    }
    
    struct tm timeInfo;
    localtime_s(&timeInfo, &timestamp);
    
    stringstream ss;
    ss << put_time(&timeInfo, "%H:%M:%S %d-%m-%Y");
    return ss.str();
}

// Display the process history
void ProcessHistory::displayHistory() {
    cout << "\nProcess History:\n";
    cout << setw(10) << left << "PID"
         << setw(30) << left << "Process Name"
         << setw(25) << left << "Start Time"
         << setw(25) << left << "End Time" << endl;
    cout << string(90, '-') << endl;
    
    for (const auto& entry : processHistory) {
        cout << setw(10) << left << entry.pid
             << setw(30) << left << entry.processName
             << setw(25) << left << formatTimestamp(entry.startTime)
             << setw(25) << left << formatTimestamp(entry.endTime) << endl;
    }
}