#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <experimental/filesystem>
#include "command_parser.h"
namespace fs = std::experimental::filesystem;

class FileManager {
public:
    static Command parseCommand(const std::string& input);
    static void executeCommand(const Command& cmd);

private:
    static void listDirectory(const std::vector<std::string>& args);
    static void changeDirectory(const std::vector<std::string>& args);
    static void makeDirectory(const std::vector<std::string>& args);
    static void removeFileOrDir(const std::vector<std::string>& args);
    static void moveFileOrDir(const std::vector<std::string>& args);
    static void copyFileOrDir(const std::vector<std::string>& args);
    static void touchFile(const std::vector<std::string>& args);
    static void writeFile(const std::vector<std::string>& args);
    static void readFile(const std::vector<std::string>& args);
};
