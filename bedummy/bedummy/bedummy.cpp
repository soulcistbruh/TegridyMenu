#include <windows.h>
#include <TlHelp32.h>

SERVICE_STATUS ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE hStatus = NULL;
bool g_shouldExit = false;

// Check if EscapeFromTarkov.exe is running
bool IsGameRunning()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return false;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    bool found = false;

    if (Process32FirstW(hSnapshot, &pe32))
    {
        do
        {
            if (_wcsicmp(pe32.szExeFile, L"EscapeFromTarkov.exe") == 0)
            {
                found = true;
                break;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return found;
}

// Service control handler - handles stop requests
VOID WINAPI ServiceCtrlHandler(DWORD dwControl)
{
    if (dwControl == SERVICE_CONTROL_STOP || dwControl == SERVICE_CONTROL_SHUTDOWN)
    {
        g_shouldExit = true;
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        ServiceStatus.dwWin32ExitCode = 0;
        SetServiceStatus(hStatus, &ServiceStatus);
    }
}

// Main service function
VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
    // Set up service status
    ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

    // Register control handler
    hStatus = RegisterServiceCtrlHandlerW(L"BEService", ServiceCtrlHandler);
    if (hStatus == NULL)
        return;

    // Tell Windows we're running
    SetServiceStatus(hStatus, &ServiceStatus);

    // Phase 1: Wait for game to start (30 minute timeout)
    bool gameDetected = false;
    DWORD waitTime = 0;
    const DWORD maxWaitTime = 30 * 60 * 1000; // 30 minutes in milliseconds

    while (!g_shouldExit && !gameDetected && waitTime < maxWaitTime)
    {
        if (IsGameRunning())
        {
            gameDetected = true;
            break;
        }
        Sleep(1000);
        waitTime += 1000;
    }

    // If game never started or we were told to stop, exit
    if (!gameDetected || g_shouldExit)
    {
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;
    }

    // Phase 2: Keep running while game is active
    while (!g_shouldExit && IsGameRunning())
    {
        Sleep(1000);
    }

    // Phase 3: Game closed or service stopped
    // Wait 5 seconds before exiting (gives bypass time to clean up)
    if (!g_shouldExit)
    {
        Sleep(5000);
    }

    // Mark service as stopped
    ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    ServiceStatus.dwWin32ExitCode = 0;
    SetServiceStatus(hStatus, &ServiceStatus);
}

// Entry point
int wmain()
{
    SERVICE_TABLE_ENTRYW ServiceTable[] = {
        { (LPWSTR)L"BEService", (LPSERVICE_MAIN_FUNCTIONW)ServiceMain },
        { NULL, NULL }
    };

    // This blocks until service is stopped
    StartServiceCtrlDispatcherW(ServiceTable);
    return 0;
}