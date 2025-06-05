#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <iomanip>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

using namespace std;

// Alias table (simple map)
static map<string, string> aliases;

void builtin_help() {
    std::cout << std::left;
    std::cout << std::setw(20) << "Command" << ": Description" << std::endl;
    std::cout << std::setw(20) << "--------------------" << "--------------------" << std::endl;
    std::cout << std::setw(20) << "Shell Command:" << std::endl;
    std::cout << std::setw(20) << "help" << ": Show this command list" << std::endl;
    std::cout << std::setw(20) << "exit" << ": Exit ThaiShell" << std::endl;
    std::cout << std::setw(20) << "--------------------" << "--------------------" << std::endl;
    std::cout << std::setw(20) << "File/Directory Command:" << std::endl;
    std::cout << std::setw(20) << "cd <dir>" << ": Change current working directory" << std::endl;
    std::cout << std::setw(20) << "ls [dir]" << ": List directory contents" << std::endl;
    std::cout << std::setw(20) << "mkdir <dir>" << ": Create a new directory" << std::endl;
    std::cout << std::setw(20) << "rm <path>" << ": Remove a file or directory" << std::endl;
    std::cout << std::setw(20) << "mv <src> <dest>" << ": Move or rename a file/directory" << std::endl;
    std::cout << std::setw(20) << "cp <src> <dest>" << ": Copy a file or directory" << std::endl;
    std::cout << std::setw(20) << "touch <file>" << ": Create or update a file" << std::endl;
    std::cout << std::setw(20) << "write <file> <text>" << ": Write text to a file (overwrites)" << std::endl;
    std::cout << std::setw(20) << "read <file>" << ": Read and display file contents" << std::endl;
    std::cout << std::setw(20) << "--------------------" << "--------------------" << std::endl;
    std::cout << std::setw(20) << "Path Command:" << std::endl;
    std::cout << std::setw(20) << "pwd" << ": Print current working directory" << std::endl;
    std::cout << std::setw(20) << "path" << ": Display ThaiShell PATH" << std::endl;
    std::cout << std::setw(20) << "path add <dir>" << ": Add directory to ThaiShell PATH" << std::endl;
    std::cout << std::setw(20) << "path remove <dir>" << ": Remove directory from ThaiShell PATH" << std::endl;
    std::cout << std::setw(20) << "--------------------" << "--------------------" << std::endl;
    std::cout << std::setw(20) << "Process Command:" << std::endl;
    std::cout << std::setw(20) << "start <processName>" << ": Start foreground process" << std::endl;
    std::cout << std::setw(20) << "start <processName> &" << ": Start background process" << std::endl;
    std::cout << std::setw(20) << "myList" << ": List background processes created by ThaiShell" << std::endl;
    std::cout << std::setw(20) << "globalList" << ": List all background processes" << std::endl;
    std::cout << std::setw(20) << "kill <pid>" << ": Kill a process by PID" << std::endl;
    std::cout << std::setw(20) << "stop <pid>" << ": Stop a process by PID" << std::endl;
    std::cout << std::setw(20) << "resume <pid>" << ": Resume a stopped process by PID" << std::endl;
    std::cout << std::setw(20) << "--------------------" << "--------------------" << std::endl;
    std::cout << std::setw(20) << "Miscellaneous Command:" << std::endl;
    std::cout << std::setw(20) << "alias name=cmd" << ": Define a command alias" << std::endl;
    std::cout << std::setw(20) << "unalias name" << ": Remove an alias" << std::endl;
    std::cout << std::setw(20) << "--------------------" << "--------------------" << std::endl;
    std::cout << std::setw(20) << "Other Command:" << std::endl;
}

void builtin_cd(const vector<string>& args) {
    if (args.size() < 2) {
        cerr << "Usage: cd <directory>\n";
        return;
    }
    try {
        fs::current_path(args[1]);
    } catch (const exception& e) {
        cerr << "cd error: " << e.what() << endl;
    }
}

bool builtin_exit() {
    cout << "Exiting ThaiShell...\n";
    return true; // Signal to exit
}

void builtin_alias(const vector<string>& args) {
    if (args.size() < 2) {
        cerr << "Usage: alias name=command\n";
        return;
    }
    string input = args[1];
    auto pos = input.find('=');
    if (pos == string::npos) {
        cerr << "Invalid alias format. Use name=command\n";
        return;
    }
    string name = input.substr(0, pos);
    string cmd = input.substr(pos + 1);
    aliases[name] = cmd;
    cout << "Alias set: " << name << "='" << cmd << "'\n";
}

void builtin_unalias(const vector<string>& args) {
    if (args.size() < 2) {
        cerr << "Usage: unalias name\n";
        return;
    }
    string name = args[1];
    if (aliases.erase(name)) {
        cout << "Removed alias: " << name << endl;
    } else {
        cerr << "Alias not found: " << name << endl;
    }
}

bool is_alias(const string& name) {
    return aliases.count(name) > 0;
}

string resolve_alias(const string& name) {
    if (is_alias(name)) return aliases[name];
    return name;
}

bool handle_builtin(vector<string>& args) {
    if (args.empty()) return false;

    string cmd = args[0];

    if (cmd == "cd") {
        builtin_cd(args);
        return true;
    } else if (cmd == "alias") {
        builtin_alias(args);
        return true;
    } else if (cmd == "unalias") {
        builtin_unalias(args);
        return true;
    } else if (cmd == "help") {
        builtin_help();
        return true;
    } else if (cmd == "exit") {
        if (builtin_exit()) {
            exit(0);
        }
        return true;
    }

    // Nếu là alias, thì thay thế args[0] bằng lệnh thật
    if (is_alias(cmd)) {
        args[0] = resolve_alias(cmd);
        return false; // Không phải built-in, tiếp tục xử lý như tiến trình thường
    }

    return false; // Không phải built-in
}
