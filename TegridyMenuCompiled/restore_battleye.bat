@echo off
setlocal enabledelayedexpansion

title EFT - Restore Real BattlEye
color 0C

echo ================================================
echo   Restore Real BattlEye Service
echo ================================================
echo.
echo This will remove the dummy service and restore
echo the real BattlEye for online play.
echo.

:: Check for admin privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo [ERROR] This script requires Administrator privileges!
    echo.
    echo Please right-click and select "Run as administrator"
    echo.
    pause
    exit /b 1
)

echo [+] Running with Administrator privileges
echo.

:: Step 1: Stop dummy service (but keep the .exe file)
echo ================================================
echo   Step 1: Stopping dummy BEService
echo ================================================
echo.

echo [*] Killing dummy BEservice.exe processes...
taskkill /f /im BEservice.exe >nul 2>&1
if %errorLevel% equ 0 (
    echo [+] Dummy process terminated
) else (
    echo [*] Dummy process not running
)

echo [*] Stopping dummy BEService...
sc stop BEService >nul 2>&1
timeout /t 1 /nobreak >nul

echo [*] Deleting dummy BEService registration...
sc delete BEService >nul 2>&1

echo [*] Cleaning up BEDaisy if present...
sc stop BEDaisy >nul 2>&1
sc delete BEDaisy >nul 2>&1

echo [+] Dummy service registration removed
echo [*] Note: BEservice.exe file kept for future offline use
echo.

timeout /t 2 /nobreak >nul

:: Step 2: Verify game installation
echo ================================================
echo   Step 2: Locating game installation
echo ================================================
echo.

:: Common installation paths
set "GAME_PATH="
set "BE_PATH="

:: Check common Steam locations
if exist "C:\Program Files (x86)\Steam\steamapps\common\Escape from Tarkov\BattlEye\BEService_x64.exe" (
    set "GAME_PATH=C:\Program Files (x86)\Steam\steamapps\common\Escape from Tarkov"
    set "BE_PATH=!GAME_PATH!\BattlEye\BEService_x64.exe"
)

:: Check common BSG launcher locations
if exist "C:\Battlestate Games\EFT\BattlEye\BEService_x64.exe" (
    set "GAME_PATH=C:\Battlestate Games\EFT"
    set "BE_PATH=!GAME_PATH!\BattlEye\BEService_x64.exe"
)

if exist "D:\Battlestate Games\EFT\BattlEye\BEService_x64.exe" (
    set "GAME_PATH=D:\Battlestate Games\EFT"
    set "BE_PATH=!GAME_PATH!\BattlEye\BEService_x64.exe"
)

if exist "E:\Battlestate Games\EFT\BattlEye\BEService_x64.exe" (
    set "GAME_PATH=E:\Battlestate Games\EFT"
    set "BE_PATH=!GAME_PATH!\BattlEye\BEService_x64.exe"
)

:: If not found, ask user
if not defined BE_PATH (
    echo [*] Could not auto-detect game installation
    echo.
    echo Please enter the full path to your EFT installation folder
    echo Example: C:\Battlestate Games\EFT
    echo.
    set /p "GAME_PATH=Game Path: "
    
    if not exist "!GAME_PATH!\BattlEye\BEService_x64.exe" (
        echo.
        echo [ERROR] BEService_x64.exe not found at:
        echo !GAME_PATH!\BattlEye\BEService_x64.exe
        echo.
        echo Please verify your game installation path.
        pause
        exit /b 1
    )
    
    set "BE_PATH=!GAME_PATH!\BattlEye\BEService_x64.exe"
)

echo [+] Game found at: !GAME_PATH!
echo [+] BattlEye path: !BE_PATH!
echo.

:: Step 3: Reinstall real BattlEye service
echo ================================================
echo   Step 3: Installing real BattlEye service
echo ================================================
echo.

echo [*] Installing BEService...
sc create BEService binPath= "!BE_PATH!" type= own start= auto DisplayName= "BattlEye Service"

if %errorLevel% neq 0 (
    echo.
    echo [ERROR] Failed to create BEService!
    echo.
    echo This might be because:
    echo   - Service already exists
    echo   - Incorrect path
    echo   - Permission issues
    echo.
    pause
    exit /b 1
)

echo [+] BEService created successfully
echo.

:: Step 4: Verify registration
echo ================================================
echo   Step 4: Verifying service registration
echo ================================================
echo.

sc query BEService >nul 2>&1
if %errorLevel% equ 0 (
    echo [+] BEService successfully registered
    echo [*] Service will start automatically when game launches
) else (
    echo [!] Warning: Could not verify service registration
    echo [*] Try running the game launcher to complete setup
)
echo.

:: Final message
echo ================================================
echo   Restoration Complete!
echo ================================================
echo.
echo [+] Real BattlEye service has been restored
echo [+] Service is registered and ready
echo [+] You can now play online normally
echo.
echo IMPORTANT:
echo   - Launch the game through the official launcher
echo   - BattlEye will start automatically
echo   - Do NOT use launch.bat for online play
echo.
echo To use offline mods again, run launch.bat
echo.
pause