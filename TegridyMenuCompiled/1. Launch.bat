@echo off
setlocal enabledelayedexpansion

title PvE Launcher
color 0A

echo ==================================
echo   PvE Launcher - BattlEye Bypass
echo ==================================
echo.
echo Credits:
echo   BEService bypass - rqhz
echo   battleyen't for il2cpp - Suchi96
echo   battleyen't and menu references - xXBattleEyeXx
echo   countless others not mentioned here..
echo   Tegridy menu - Soulcist/claude ai
echo.
echo This is free on UnknownCheats.me
echo.

:: Check for admin privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo [ERROR] This launcher requires Administrator privileges!
    echo.
    echo Please right-click launch.bat and select "Run as administrator"
    echo.
    pause
    exit /b 1
)

echo [+] Running with Administrator privileges
echo.

:: Step 1: Cleanup
echo ================================================
echo   Step 1: Cleaning up existing services
echo ================================================
echo.

echo [*] Killing any existing BEservice.exe processes...
taskkill /f /im BEservice.exe >nul 2>&1
timeout /t 1 /nobreak >nul

echo [*] Stopping BEService...
sc stop BEService >nul 2>&1
timeout /t 1 /nobreak >nul

echo [*] Deleting BEService...
sc delete BEService >nul 2>&1
timeout /t 1 /nobreak >nul

echo [*] Stopping BEDaisy...
sc stop BEDaisy >nul 2>&1

echo [*] Deleting BEDaisy...
sc delete BEDaisy >nul 2>&1

echo [+] Cleanup complete!
echo.

timeout /t 2 /nobreak >nul

:: Step 2: Create dummy BEService
echo ================================================
echo   Step 2: Creating dummy BEService
echo ================================================
echo.

set "SERVICE_PATH=%~dp0BEservice.exe"

if not exist "%SERVICE_PATH%" (
    echo [ERROR] BEservice.exe not found at:
    echo %SERVICE_PATH%
    echo.
    echo Make sure BEservice.exe is in the same folder as this batch file!
    pause
    exit /b 1
)

echo [*] Service path: %SERVICE_PATH%
echo [*] Creating service...

sc create BEService binPath= "%SERVICE_PATH%" type= own start= auto DisplayName= "BattlEye Service"

if %errorLevel% neq 0 (
    echo.
    echo [ERROR] Failed to create service! Error code: %errorLevel%
    echo.
    echo Possible causes:
    echo   - Service already exists
    echo   - Path contains invalid characters
    echo   - Not running as Administrator
    echo.
    echo Trying to query existing service...
    sc query BEService
    echo.
    pause
    exit /b 1
)

echo [+] Service created successfully!
echo.

:: Step 3: Start the service
echo ================================================
echo   Step 3: Starting BEService
echo ================================================
echo.

echo [*] Starting service...
sc start BEService

if %errorLevel% neq 0 (
    if %errorLevel%==1056 (
        echo [*] Service already running, continuing...
    ) else (
        echo.
        echo [ERROR] Failed to start service! Error code: %errorLevel%
        echo.
        sc query BEService
        echo.
        pause
        exit /b 1
    )
) else (
    echo [+] Service started!
)
echo.

:: Step 4: Verify service is running
echo ================================================
echo   Step 4: Verifying service status
echo ================================================
echo.

echo [*] Checking service status...
set "SERVICE_RUNNING=0"
set "RETRY_COUNT=0"

:CHECK_SERVICE
sc query BEService | find "RUNNING" >nul 2>&1
if %errorLevel% equ 0 (
    set "SERVICE_RUNNING=1"
    goto SERVICE_VERIFIED
)

set /a RETRY_COUNT+=1
if %RETRY_COUNT% geq 20 (
    echo [ERROR] Service not running after 5 seconds!
    echo.
    echo Current service status:
    sc query BEService
    echo.
    pause
    exit /b 1
)

timeout /t 1 /nobreak >nul
goto CHECK_SERVICE

:SERVICE_VERIFIED
echo [+] Service verified - STATUS: RUNNING
echo.

:: Additional stabilization wait
echo [*] Stabilizing service...
timeout /t 2 /nobreak >nul
echo [+] Service is stable and ready!
echo.

:: Step 5: Launch battleyent.exe (DLL injector)
echo ====================================
echo   Step 5: Launching battleyent.exe
echo ====================================
echo.

set "INJECTOR_PATH=%~dp0battleyent.exe"

if not exist "%INJECTOR_PATH%" (
    echo [ERROR] battleyent.exe not found at:
    echo %INJECTOR_PATH%
    echo.
    pause
    exit /b 1
)

echo [*] Launching DLL injector...
start "" "%INJECTOR_PATH%"

if %errorLevel% neq 0 (
    echo [ERROR] Failed to launch battleyent.exe!
    pause
    exit /b 1
)

echo [+] Injector launched!
echo.

:: Done - battleyent.exe will handle game detection and injection
echo ====================
echo   Setup Complete!
echo ====================
echo.
echo [+] BEService is running
echo [+] battleyent.exe has been launched
echo [+] Service will auto-terminate when game closes
echo.
echo You can now launch the game.
echo This window will close in 3 seconds...
echo.

timeout /t 2 /nobreak >nul
exit /b 0