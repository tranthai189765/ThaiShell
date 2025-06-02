// main.cpp for ThaiShell
#include <iostream>
#include <string>
#include <vector>
#include "command_parser.h"
#include "process_executor.h"
#include "process_manager.h"
#include "logger.h"
#include "builtins.h"
using namespace std;
int main() {
    printBanner();
    ProcessManager procManager;
    string input;

    while (true) {
        cout << "[ThaiShell] ";
        getline(cin, input);
        if (input.empty()) continue;

        Command cmd = CommandParser::parse(input);

        // Ghép command và arguments lại để truyền vào handle_builtin
        vector<string> fullArgs = {cmd.program};
        fullArgs.insert(fullArgs.end(), cmd.args.begin(), cmd.args.end());

        if (handle_builtin(fullArgs)) continue;

        if (procManager.handleCommand(cmd)) {
            continue;
        }

        ProcessExecutor::execute(cmd, procManager);
    }
}

