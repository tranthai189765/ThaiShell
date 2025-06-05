#pragma once
#include <string>
#include <vector>
#include <experimental/filesystem>
#include "command_parser.h"
namespace fs = std::experimental::filesystem;

class PathManager {
public:
    static void executeCommand(const Command& cmd);

private:
    static std::vector<std::string> thaiShellPath; // ThaiShell-specific PATH
    static void printWorkingDirectory(const std::vector<std::string>& args);
    static void managePath(const std::vector<std::string>& args);
};
