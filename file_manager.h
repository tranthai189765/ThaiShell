#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <experimental/filesystem>
#include "command_parser.h"
namespace fs = std::experimental::filesystem;
using namespace std;

class FileManager {
public:
    static Command parseCommand(const string& input);
    static void executeCommand(const Command& cmd);

private:
    static void listDirectory(const vector<string>& args);
    static void changeDirectory(const vector<string>& args);
    static void makeDirectory(const vector<string>& args);
    static void removeFileOrDir(const vector<string>& args);
    static void moveFileOrDir(const vector<string>& args);
    static void copyFileOrDir(const vector<string>& args);
    static void touchFile(const vector<string>& args);
    static void writeFile(const vector<string>& args);
    static void readFile(const vector<string>& args);
};
