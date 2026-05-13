@echo off
REM Check if VCPKG_ROOT environment variable is set
IF "%VCPKG_ROOT%"=="" (
    echo VCPKG_ROOT environment variable not detected. Please set VCPKG_ROOT first.
    exit /b 1
)

REM Check if vcpkg.exe exists in VCPKG_ROOT directory
IF NOT EXIST "%VCPKG_ROOT%\vcpkg.exe" (
    echo vcpkg.exe not found in %VCPKG_ROOT%. Please check your VCPKG_ROOT setting.
    exit /b 1
)

REM Check if package list file exists
IF NOT EXIST "VcpkgPackageList.txt" (
    echo VcpkgPackageList.txt not found in current directory.
    exit /b 1
)

REM Read packages from packages.txt and install them
setlocal enabledelayedexpansion
set PACKAGES=
for /f "usebackq tokens=*" %%i in ("VcpkgPackageList.txt") do (
    set PKG=%%i
    if not "!PKG!"=="" (
        set PACKAGES=!PACKAGES! !PKG!
    )
)

if "%PACKAGES%"=="" (
    echo No packages specified in VcpkgPackageList.txt.
    exit /b 1
)

call "%VCPKG_ROOT%\vcpkg.exe" install %PACKAGES%

REM Check installation result
IF %ERRORLEVEL%==0 (
    echo Packages installed successfully.
    pause
) ELSE (
    echo Package installation failed. Please check the error message.
    pause
    exit /b 1
)

pause