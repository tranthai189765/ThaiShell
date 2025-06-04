#pragma once

// Prevent Windows headers from defining conflicting 'byte' type
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WINDOWS_LEAN_AND_MEAN
#define WINDOWS_LEAN_AND_MEAN
#endif

#include "command_parser.h"
#include "process_manager.h"
class ProcessExecutor {
public:
    static void execute(const Command& cmd, ProcessManager& procManager);
};

