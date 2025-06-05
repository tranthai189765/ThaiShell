#include "file_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "color.h"
Command FileManager::parseCommand(const string& input) {
    Command cmd;
    cmd.isBackground = false;
    string trimmed = input;
    while (!trimmed.empty() && trimmed.back() == ' ') {
        trimmed.pop_back();
    }
    if (!trimmed.empty() && trimmed.back() == '&') {
        cmd.isBackground = true;
        trimmed.pop_back();
        while (!trimmed.empty() && trimmed.back() == ' ') {
            trimmed.pop_back();
        }
    }
    string token;
    bool inQuotes = false;
    for (char c : trimmed) {
        if (c == '"') {
            inQuotes = !inQuotes;
            continue;
        }
        if (c == ' ' && !inQuotes) {
            if (!token.empty()) {
                if (cmd.program.empty()) {
                    cmd.program = token;
                } else {
                    cmd.args.push_back(token);
                }
                token.clear();
            }
            continue;
        }
        token += c;
    }
    if (!token.empty()) {
        if (cmd.program.empty()) {
            cmd.program = token;
        } else {
            cmd.args.push_back(token);
        }
    }
    return cmd;
}

void FileManager::executeCommand(const Command& cmd) {
    if (cmd.program == "ls" || cmd.program == "dir") {
        listDirectory(cmd.args);
    } else if (cmd.program == "cd") {
        changeDirectory(cmd.args);
    } else if (cmd.program == "mkdir") {
        makeDirectory(cmd.args);
    } else if (cmd.program == "rm") {
        removeFileOrDir(cmd.args);
    } else if (cmd.program == "mv") {
        moveFileOrDir(cmd.args);
    } else if (cmd.program == "cp") {
        copyFileOrDir(cmd.args);
    } else if (cmd.program == "touch") {
        touchFile(cmd.args);
    } else if (cmd.program == "write") {
        writeFile(cmd.args);
    } else if (cmd.program == "read") {
        readFile(cmd.args);
    } else {
        cout << "Command not found: " << cmd.program << endl;
    }
}

void FileManager::listDirectory(const vector<string>& args) {
    fs::path path = args.empty() ? fs::current_path() : args[0];
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            cout << (fs::is_directory(entry.status()) ? COLOR_CYAN "[DIR]  " COLOR_RESET : COLOR_YELLOW "[FILE] " COLOR_RESET)
                 << entry.path().filename().string() << endl;
        }
    } catch (const fs::filesystem_error& e) {
        cout << "Error listing directory: " << e.what() << endl;
    }
}

void FileManager::changeDirectory(const vector<string>& args) {
    if (args.empty()) {
        cout << "Current directory: " << fs::current_path().string() << endl;
        return;
    }
    try {
        fs::current_path(args[0]);
        cout << "Changed to: " << fs::current_path().string() << endl;
    } catch (const fs::filesystem_error& e) {
        cout << "Error changing directory: " << e.what() << endl;
    }
}

void FileManager::makeDirectory(const vector<string>& args) {
    if (args.empty()) {
        cout << "mkdir: missing directory name" << endl;
        return;
    }
    try {
        if (fs::create_directories(args[0])) {
            cout << "Directory created: " << args[0] << endl;
        } else {
            cout << "Directory already exists: " << args[0] << endl;
        }
    } catch (const fs::filesystem_error& e) {
        cout << "Error creating directory: " << e.what() << endl;
    }
}

void FileManager::removeFileOrDir(const vector<string>& args) {
    if (args.empty()) {
        cout << "rm: missing file/directory name" << endl;
        return;
    }
    try {
        if (fs::remove_all(args[0])) {
            cout << "Removed: " << args[0] << endl;
        } else {
            cout << "Nothing to remove: " << args[0] << endl;
        }
    } catch (const fs::filesystem_error& e) {
        cout << "Error removing: " << e.what() << endl;
    }
}

void FileManager::moveFileOrDir(const vector<string>& args) {
    if (args.size() < 2) {
        cout << "mv: missing source or destination" << endl;
        return;
    }
    try {
        fs::rename(args[0], args[1]);
        cout << "Moved " << args[0] << " to " << args[1] << endl;
    } catch (const fs::filesystem_error& e) {
        cout << "Error moving: " << e.what() << endl;
    }
}

void FileManager::copyFileOrDir(const vector<string>& args) {
    if (args.size() < 2) {
        cout << "cp: missing source or destination" << endl;
        return;
    }
    try {
        if (fs::is_directory(args[0])) {
            fs::copy(args[0], args[1], fs::copy_options::recursive);
        } else {
            fs::copy_file(args[0], args[1]);
        }
        cout << "Copied " << args[0] << " to " << args[1] << endl;
    } catch (const fs::filesystem_error& e) {
        cout << "Error copying: " << e.what() << endl;
    }
}

void FileManager::touchFile(const vector<string>& args) {
    if (args.empty()) {
        cout << "touch: missing file name" << endl;
        return;
    }
    try {
        ofstream ofs(args[0], ios::app);
        if (ofs) {
            cout << "Created/Updated: " << args[0] << endl;
            ofs.close();
        } else {
            cout << "Error creating file: " << args[0] << endl;
        }
    } catch (const exception& e) {
        cout << "Error touching file: " << e.what() << endl;
    }
}

void FileManager::writeFile(const vector<string>& args) {
    if (args.size() < 2) {
        cout << "write: missing file name or text" << endl;
        return;
    }
    try {
        ofstream ofs(args[0], ios::out); // Overwrite mode
        if (!ofs) {
            cout << "Error opening file: " << args[0] << endl;
            return;
        }
        string text;
        for (size_t i = 1; i < args.size(); ++i) {
            text += args[i];
            if (i < args.size() - 1) text += " ";
        }
        ofs << text << endl;
        if (ofs.good()) {
            cout << "Wrote to file: " << args[0] << endl;
        } else {
            cout << "Error writing to file: " << args[0] << endl;
        }
        ofs.close();
    } catch (const exception& e) {
        cout << "Error writing to file: " << e.what() << endl;
    }
}

void FileManager::readFile(const vector<string>& args) {
    if (args.empty()) {
        cout << "read: missing file name" << endl;
        return;
    }
    try {
        ifstream ifs(args[0]);
        if (!ifs) {
            cout << "Error opening file: " << args[0] << endl;
            return;
        }
        string line;
        while (getline(ifs, line)) {
            cout << line << endl;
        }
        ifs.close();
    } catch (const exception& e) {
        cout << "Error reading file: " << e.what() << endl;
    }
}

