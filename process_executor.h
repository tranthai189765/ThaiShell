#pragma once
#include "command_parser.h"
#include "process_manager.h"
class ProcessExecutor {
public:
    static void execute(const Command& cmd, ProcessManager& procManager);
};

