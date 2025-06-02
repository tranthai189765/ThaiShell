#pragma once
#include <string>
#include <vector>
using namespace std;
struct Command {
    string program;
    vector<string> args;
    bool isBackground;
};

class CommandParser {
public:
    static Command parse(const string& input);
};
