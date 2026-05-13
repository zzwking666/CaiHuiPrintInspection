@echo off
setlocal enabledelayedexpansion

REM Configuration parameters
set ServerIP=192.168.2.63
set Username=zfkjStorage
set RemotePath=/home/zfkjStorage/DevelopEnvironment/StorageForRW_UL/ThirdLibrary.zip

REM Use absolute paths
set LocalDir=D:\zfkjDevelopment
set LocalPath=D:\zfkjDevelopment\ThirdLibrary.zip

echo Absolute local directory: %LocalDir%
echo Absolute local zip path: %LocalPath%

REM Create local directory if it does not exist
if not exist "%LocalDir%" (
    mkdir "%LocalDir%"
)

REM Check if the target file exists, delete if it does
if exist "%LocalPath%" (
    echo Target file already exists, deleting...
    del /f /q "%LocalPath%"
    if exist "%LocalPath%" (
        echo Delete failed
        pause
        exit /b 1
    ) else (
        echo Old file deleted successfully
    )
)

REM Clean up previously extracted folders (do not delete zip)
REM Delete folders
for /d %%D in ("%LocalDir%\*") do (
    if /I not "%%~xD"==".zip" (
        echo Deleting folder: %%D
        rd /s /q "%%D"
    )
)
REM Delete files
for %%F in ("%LocalDir%\*") do (
    if /I not "%%~xF"==".zip" (
        echo Deleting file: %%F
        del /f /q "%%F"
    )
)

REM Download file
echo Downloading file using SCP...
scp "%Username%@%ServerIP%:%RemotePath%" "%LocalPath%"
if errorlevel 1 (
    echo Download failed!
    pause
    exit /b 1
) else (
    echo File downloaded successfully: %LocalPath%
)

REM Extract zip file (temporarily bypass execution policy)
echo Extracting zip file...
powershell -ExecutionPolicy Bypass -Command "Expand-Archive -Path '%LocalPath%' -DestinationPath '%LocalDir%' -Force"
if errorlevel 1 (
    echo Extraction failed!
    pause
    exit /b 1
) else (
    echo Extraction succeeded: %LocalDir%
)

REM Delete downloaded zip file
del /f /q "%LocalPath%"

echo zip cleaned up
pause
exit /b 0