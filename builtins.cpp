#include "builtins.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <map>

using namespace std;
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

// Alias table (simple map)
static map<string, string> aliases;

void builtin_help() {
    cout << "\nAvailable ThaiShell built-in commands:\n"
         << "  help             - Show this help message\n"
         << "  cd <dir>         - Change current working directory\n"
         << "  exit             - Exit ThaiShell\n"
         << "  jobs             - List background processes\n"
         << "  alias name=cmd   - Define a command alias\n"
         << "  unalias name     - Remove an alias\n"
         << endl;
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
