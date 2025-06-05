#ifndef BUILTINS_H
#define BUILTINS_H

#include <vector>
#include <string>

// Built-in command handlers
void builtin_help();
void builtin_cd(const std::vector<std::string>& args);
bool builtin_exit();
void builtin_alias(const std::vector<std::string>& args);
void builtin_unalias(const std::vector<std::string>& args);
void builtin_echo(const std::vector<std::string>& args);

// Alias resolution
bool is_alias(const std::string& name);
std::string resolve_alias(const std::string& name);

// New: unified handler
bool handle_builtin(std::vector<std::string>& args); // return true if it's a built-in command

#endif
