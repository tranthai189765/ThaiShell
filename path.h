#pragma once
#include <string>
#include <vector>
#include <experimental/filesystem>
#include "command_parser.h"
using namespace std;
namespace fs = std::experimental::filesystem;

class PathManager {
public:
    static void executeCommand(const Command& cmd);

private:
    static vector<string> thaiShellPath; // ThaiShell-specific PATH
    static void printWorkingDirectory(const vector<string>& args);
    static void managePath(const vector<string>& args);
};
