// Windows macros are defined on command line with -D flag

#include <iostream>
#include <string>
#include <vector>
#include "command_parser.h"
#include "process_executor.h"
#include "process_manager.h"
#include "logger.h"
#include "builtins.h"
#include "file_manager.h"
#include "path.h"
#include "color.h"
#include "ctrl_c_handler.h"
using namespace std;

int main() {
    enableANSISupport(); // Enable ANSI colors on Windows
    
    // Initialize the Ctrl+C handler
    CtrlCHandler::initialize();
    
    printInitialBanner();
    ProcessManager procManager;
    string input;while (true) {
        cout << COLOR_GREEN "[ThaiShell]" COLOR_RESET " > ";
        getline(cin, input);
        if (input.empty()) continue;

        Command cmd = FileManager::parseCommand(input);

        // Ghép command và arguments lại để truyền vào handle_builtin
        vector<string> fullArgs = {cmd.program};
        fullArgs.insert(fullArgs.end(), cmd.args.begin(), cmd.args.end());

        if (handle_builtin(fullArgs)) continue;

        if (cmd.program == "ls" || cmd.program == "dir" ||
            cmd.program == "mkdir" || cmd.program == "rm" ||
            cmd.program == "mv" || cmd.program == "cp" ||
            cmd.program == "touch" || cmd.program == "write" ||
            cmd.program == "read") {
            FileManager::executeCommand(cmd);
            continue;
        }

        if (cmd.program == "pwd" || cmd.program == "path") {
            PathManager::executeCommand(cmd);
            continue;
        }

        if (procManager.handleCommand(cmd)) {
            continue;
        }

        ProcessExecutor::execute(cmd, procManager);
    }
}
