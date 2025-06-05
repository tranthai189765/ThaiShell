#include "path.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

// Initialize ThaiShell-specific PATH
vector<string> PathManager::thaiShellPath;

void PathManager::executeCommand(const Command& cmd) {
    if (cmd.program == "pwd") {
        printWorkingDirectory(cmd.args);
    } else if (cmd.program == "path") {
        managePath(cmd.args);
    } else {
        cout << "Path command not found: " << cmd.program << endl;
    }
}

void PathManager::printWorkingDirectory(const vector<string>& args) {
    if (!args.empty()) {
        cout << "pwd: no arguments expected" << endl;
        return;
    }
    try {
        cout << fs::current_path().string() << endl;
    } catch (const fs::filesystem_error& e) {
        cout << "Error getting current directory: " << e.what() << endl;
    }
}

void PathManager::managePath(const vector<string>& args) {
    if (args.empty()) {
        // Display ThaiShell PATH
        if (thaiShellPath.empty()) {
            cout << "ThaiShell PATH is empty" << endl;
        } else {
            cout << "ThaiShell PATH:" << endl;
            for (size_t i = 0; i < thaiShellPath.size(); ++i) {
                cout << "  " << setw(2) << i + 1 << ". " << thaiShellPath[i] << endl;
            }
        }
        return;
    }

    if (args[0] == "add" && args.size() >= 2) {
        // Add directory to ThaiShell PATH
        string new_dir = args[1];
        if (!fs::exists(new_dir)) {
            cout << "path add: directory does not exist: " << new_dir << endl;
            return;
        }
        // Check if directory is already in PATH
        if (find(thaiShellPath.begin(), thaiShellPath.end(), new_dir) != thaiShellPath.end()) {
            cout << "path add: directory already in ThaiShell PATH: " << new_dir << endl;
            return;
        }
        thaiShellPath.push_back(new_dir);
        cout << "Added to ThaiShell PATH: " << new_dir << endl;
    } else if (args[0] == "remove" && args.size() >= 2) {
        // Remove directory from ThaiShell PATH
        string dir_to_remove = args[1];
        auto it = find(thaiShellPath.begin(), thaiShellPath.end(), dir_to_remove);
        if (it == thaiShellPath.end()) {
            cout << "path remove: directory not found in ThaiShell PATH: " << dir_to_remove << endl;
            return;
        }
        thaiShellPath.erase(it);
        cout << "Removed from ThaiShell PATH: " << dir_to_remove << endl;
    } else {
        cout << "Usage: path [add <dir> | remove <dir>]" << endl;
    }
}
