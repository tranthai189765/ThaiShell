@echo off
setlocal

:: Go to the folder where this script is located
cd /d "%~dp0"

echo Terminating any running ThaiShell processes...
taskkill /F /IM ThaiShell.exe 2>NUL

echo Building ThaiShell...
g++ main.cpp builtins.cpp command_parser.cpp process_executor.cpp process_manager.cpp logger.cpp history.cpp file_manager.cpp path.cpp ctrl_c_handler.cpp -o ThaiShell.exe -std=c++17 -lstdc++fs

if %ERRORLEVEL% == 0 (
    echo Build successful!
    
    :: Create a separate launch script
    echo @echo off > run_shell.bat
    echo ThaiShell.exe >> run_shell.bat
    
) else (
    echo Build failed with error code %ERRORLEVEL%
    pause
)
