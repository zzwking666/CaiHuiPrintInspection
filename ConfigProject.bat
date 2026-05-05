@echo off
setlocal

REM Check if Git is installed
git --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Git is not installed. Please install Git first.
    exit /b 1
)

REM Set the target directory
set TARGET_DIR=ThirdRep/RW_UL

REM Check if the target directory exists
if exist "%TARGET_DIR%" (
    echo Clearing target directory: %TARGET_DIR%
    rmdir /s /q "%TARGET_DIR%"
)

REM Create the target directory
echo Creating target directory: %TARGET_DIR%
mkdir "%TARGET_DIR%"

REM Clone the repository to the target directory
echo Cloning repository to %TARGET_DIR%...
git clone https://github.com/WIND-ROAD-RUN/RW_UL.git "%TARGET_DIR%"

REM Check the result of the clone operation
if %ERRORLEVEL% EQU 0 (
    echo Repository cloned successfully!
) else (
    echo Repository clone failed. Please check your network connection or repository URL.
    exit /b 1
)

endlocal

REM Wait for user input before exiting
echo Press any key to exit...
pause >nul