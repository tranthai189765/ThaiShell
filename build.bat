@echo off
setlocal

:: Go to the folder where this script is located
cd /d "%~dp0"

echo Building ThaiShell...
g++ main.cpp builtins.cpp command_parser.cpp process_executor.cpp process_manager.cpp logger.cpp history.cpp -o ThaiShell.exe -std=c++17 -lstdc++fs

if %ERRORLEVEL% == 0 (
    echo Build successful!
    echo Running ThaiShell...
    ThaiShell.exe
) else (
    echo Build failed with error code %ERRORLEVEL%
    pause
)
