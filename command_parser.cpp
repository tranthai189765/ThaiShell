#include "command_parser.h"
#include <sstream>
#include <string>
using namespace std;
Command CommandParser::parse(const string& inputRaw){
    string input = inputRaw;
    bool isBackground = false;

    if(!input.empty() && input.back() == '&') {
        isBackground = true;
        input.pop_back();
    }

    istringstream iss(input);
    string token;
    vector<string> tokens;

    while(iss >> token) {
        tokens.push_back(token);
    }

    if(tokens.empty()) return {"", {}, false };

    return {
        tokens[0],
        vector<string>(tokens.begin() + 1, tokens.end()),
        isBackground
    };
}
