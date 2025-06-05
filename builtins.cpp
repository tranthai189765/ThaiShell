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
    int x = 30;
    std::cout << std::left;
    std::cout << std::setw(x) << "Command" << "Description" << std::endl;
    std::cout << std::setw(x) << "-------------------------------------------------------------------------" << std::endl;    
    std::cout << std::setw(x) << "Shell Command:" << std::endl;    
    std::cout << std::setw(x) << "help" << ": Show this command list" << std::endl;
    std::cout << std::setw(x) << "exit" << ": Exit ThaiShell" << std::endl;
    std::cout << std::setw(x) << "echo [-n] [-e] [text]" << ": Display a line of text" << std::endl;
    std::cout << std::setw(x) << "echo -n" << ": Do not output the trailing newline" << std::endl;
    std::cout << std::setw(x) << "echo -e" << ": Enable interpretation of backslash escapes" << std::endl;
    std::cout << std::setw(x) << "-------------------------------------------------------------------------" << std::endl;
    std::cout << std::setw(x) << "File/Directory Command:" << std::endl;
    std::cout << std::setw(x) << "cd <dir>" << ": Change current working directory" << std::endl;
    std::cout << std::setw(x) << "ls [dir]" << ": List directory contents" << std::endl;
    std::cout << std::setw(x) << "mkdir <dir>" << ": Create a new directory" << std::endl;
    std::cout << std::setw(x) << "rm <path>" << ": Remove a file or directory" << std::endl;
    std::cout << std::setw(x) << "mv <src> <dest>" << ": Move or rename a file/directory" << std::endl;
    std::cout << std::setw(x) << "cp <src> <dest>" << ": Copy a file or directory" << std::endl;
    std::cout << std::setw(x) << "touch <file>" << ": Create or update a file" << std::endl;
    std::cout << std::setw(x) << "write <file> <text>" << ": Write text to a file (overwrites)" << std::endl;
    std::cout << std::setw(x) << "read <file>" << ": Read and display file contents" << std::endl;
    std::cout << std::setw(x) << "-------------------------------------------------------------------------" << std::endl;
    std::cout << std::setw(x) << "Path Command:" << std::endl;
    std::cout << std::setw(x) << "pwd" << ": Print current working directory" << std::endl;
    std::cout << std::setw(x) << "path" << ": Display ThaiShell PATH" << std::endl;
    std::cout << std::setw(x) << "path add <dir>" << ": Add directory to ThaiShell PATH" << std::endl;
    std::cout << std::setw(x) << "path remove <dir>" << ": Remove directory from ThaiShell PATH" << std::endl;
    std::cout << std::setw(x) << "-------------------------------------------------------------------------" << std::endl;
    std::cout << std::setw(x) << "Process Command:" << std::endl;
    std::cout << std::setw(x) << "start <processName>" << ": Start foreground process" << std::endl;
    std::cout << std::setw(x) << "start <processName> &" << ": Start background process" << std::endl;
    std::cout << std::setw(x) << "myList" << ": List background processes created by ThaiShell" << std::endl;
    std::cout << std::setw(x) << "globalList" << ": List all background processes" << std::endl;
    std::cout << std::setw(x) << "kill <pid>" << ": Kill a process by PID" << std::endl;
    std::cout << std::setw(x) << "stop <pid>" << ": Stop a process by PID" << std::endl;
    std::cout << std::setw(x) << "resume <pid>" << ": Resume a stopped process by PID" << std::endl;
    std::cout << std::setw(x) << "-------------------------------------------------------------------------" << std::endl;
    std::cout << std::setw(x) << "Miscellaneous Command:" << std::endl;
    std::cout << std::setw(x) << "alias name=cmd" << ": Define a command alias" << std::endl;
    std::cout << std::setw(x) << "unalias name" << ": Remove an alias" << std::endl;
    std::cout << std::setw(x) << "-------------------------------------------------------------------------" << std::endl;
    std::cout << std::setw(x) << "Other Command:" << std::endl;
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

// Echo command implementation
void builtin_echo(const vector<string>& args) {
    // Skip the command name (args[0])
    bool add_newline = true;  // Default behavior is to add a newline
    bool interpret_escapes = false;
    size_t start_idx = 1;

    // Check for options
    while (start_idx < args.size() && args[start_idx].size() > 0 && args[start_idx][0] == '-') {
        if (args[start_idx] == "-n") {
            // Note: In standard Unix shells, -n suppresses the newline
            // In ThaiShell, we ignore it and always add a newline for consistency
            start_idx++;
        } else if (args[start_idx] == "-e") {
            interpret_escapes = true;
            start_idx++;
        } else if (args[start_idx] == "--") {
            start_idx++;
            break;
        } else {
            // If not a recognized option, assume it's part of the text
            break;
        }
    }

    // Process arguments
    for (size_t i = start_idx; i < args.size(); i++) {
        string text = args[i];
        
        // Handle escape sequences if -e is specified
        if (interpret_escapes) {
            string result;
            for (size_t j = 0; j < text.size(); j++) {
                if (text[j] == '\\' && j + 1 < text.size()) {
                    char next = text[j + 1];
                    switch (next) {
                        case 'n': result += '\n'; break;
                        case 't': result += '\t'; break;
                        case 'r': result += '\r'; break;
                        case 'a': result += '\a'; break;
                        case 'b': result += '\b'; break;
                        case 'f': result += '\f'; break;
                        case 'v': result += '\v'; break;
                        case '\\': result += '\\'; break;
                        default: result += '\\'; result += next;
                    }
                    j++; // Skip the next character
                } else {
                    result += text[j];
                }
            }
            cout << result;
        } else {
            cout << text;
        }

        // Add a space between arguments (but not after the last one)
        if (i < args.size() - 1) {
            cout << " ";
        }
    }
      // Always add a newline in ThaiShell (ignoring the add_newline flag)
    cout << endl;
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
    } else if (cmd == "echo") {
        builtin_echo(args);
        return true;
    }

    // Nếu là alias, thì thay thế args[0] bằng lệnh thật
    if (is_alias(cmd)) {
        args[0] = resolve_alias(cmd);
        return false; // Không phải built-in, tiếp tục xử lý như tiến trình thường
    }

    return false; // Không phải built-in
}
