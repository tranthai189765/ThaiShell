#pragma once

// Prevent Windows headers from defining conflicting 'byte' type
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define WINDOWS_LEAN_AND_MEAN

#include "command_parser.h"
#include "process_manager.h"
class ProcessExecutor {
public:
    static void execute(const Command& cmd, ProcessManager& procManager);
};

