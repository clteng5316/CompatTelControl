#ifndef UNICODE
#define UNICODE
#endif
//
#ifndef _UNICODE
#define _UNICODE
#endif
//
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
//
#ifndef WINVER
#define WINVER 0x0601
#endif
//
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <wchar.h>
//
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
//
#define APP_NAME                                        L"CompatTelControl"
#define APP_VERSION                                     L"1.1"
#define APP_AUTHOR                                      L"Teng Chuan-Liang"
#define COPYRIGHT_TEXT                                  L"Copyright (C) 2026 Teng Chuan-Liang. All rights reserved."
//
static const wchar_t *TASK_APPRAISER   = L"\\Microsoft\\Windows\\Application Experience\\Microsoft Compatibility Appraiser";
static const wchar_t *TASK_PROGRAMDATA = L"\\Microsoft\\Windows\\Application Experience\\ProgramDataUpdater";
//
static BOOL IsAdministrator(void)
{
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    BOOL                     isAdmin     = FALSE;
    PSID                     adminGroup  = NULL;
//
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup))
    {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    };
//
    return isAdmin;
};
//
static BOOL RelaunchAsAdministrator(void)
{
    wchar_t   exePath[MAX_PATH] = {0};
    HINSTANCE result;
//
    if (!GetModuleFileNameW(NULL, exePath, MAX_PATH))
        return FALSE;
//
    result = ShellExecuteW(NULL, L"runas", exePath,NULL, NULL, SW_SHOWNORMAL);
//
    return ((INT_PTR)result > 32);
};
//
static DWORD RunCommand(const wchar_t *command)
{
    STARTUPINFOW        si;
    PROCESS_INFORMATION pi;
    wchar_t             cmdLine[2048] = {0};
    DWORD               exitCode      = (DWORD)-1;
//
    RtlZeroMemory(&si, sizeof(si));
    RtlZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
//
    _snwprintf(cmdLine, 2047, L"cmd.exe /c %s", command);
    cmdLine[2047] = L'\0';
//
    if (!CreateProcessW(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        wprintf(L"[ERROR] CreateProcess failed. Win32 error = %lu\n", GetLastError());
        return (DWORD)-1;
    };
//
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exitCode);
//
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
//
    return exitCode;
};
//
static void KillCompatTelRunner(void)
{
    DWORD result;
//
    result = RunCommand(L"taskkill.exe /F /IM CompatTelRunner.exe >nul 2>&1");
//
    if (result == 0)
        wprintf(L"[OK] CompatTelRunner.exe terminated.\n");
    else
        wprintf(L"[INFO] CompatTelRunner.exe was not running, or Windows refused termination.\n");
};
//
static void EndScheduledTask(const wchar_t *taskName)
{
    wchar_t command[1024] = {0};

    _snwprintf(command, 1023, L"schtasks.exe /End /TN \"%s\"", taskName);
    command[1023] = L'\0';
    RunCommand(command);
};
//
static DWORD SetTaskEnabled(const wchar_t *taskName, BOOL enable)
{
    wchar_t command[1024] = {0};
//
    _snwprintf(command, 1023, L"schtasks.exe /Change /TN \"%s\" /%s", taskName, enable ? L"ENABLE" : L"DISABLE");
    command[1023] = L'\0';
    return RunCommand(command);
};
//
static void QueryTask(const wchar_t *taskName)
{
    wchar_t command[1024] = {0};
//
    _snwprintf(command, 1023, L"schtasks.exe /Query /TN \"%s\" /FO LIST /V", taskName);
    command[1023] = L'\0';
    RunCommand(command);
};
//
static void DisableCompatTelemetry(void)
{
    DWORD r1, r2;
//
    wprintf(L"\n=== Disable Compatibility Appraiser ===\n");
//
    EndScheduledTask(TASK_APPRAISER);
    EndScheduledTask(TASK_PROGRAMDATA);
//
    KillCompatTelRunner();
//
    r1 = SetTaskEnabled(TASK_APPRAISER, FALSE);
    r2 = SetTaskEnabled(TASK_PROGRAMDATA, FALSE);
//
    KillCompatTelRunner();
//
    wprintf(L"\nResult:\n");
    wprintf(L"  Microsoft Compatibility Appraiser : %s\n",  (r1 == 0) ? L"disabled" : L"not changed / task not present");
    wprintf(L"  ProgramDataUpdater                 : %s\n", (r2 == 0) ? L"disabled" : L"not changed / task not present");
//
    wprintf(L"\nA Windows feature update may recreate or re-enable these tasks.\n");
};
//
static void EnableCompatTelemetry(void)
{
    DWORD r1, r2;
//
    wprintf(L"\n=== Restore Compatibility Appraiser ===\n");
//
    r1 = SetTaskEnabled(TASK_APPRAISER, TRUE);
    r2 = SetTaskEnabled(TASK_PROGRAMDATA, TRUE);
//
    wprintf(L"\nResult:\n");
    wprintf(L"  Microsoft Compatibility Appraiser : %s\n",  (r1 == 0) ? L"enabled" : L"not changed / task not present");
    wprintf(L"  ProgramDataUpdater                 : %s\n", (r2 == 0) ? L"enabled" : L"not changed / task not present");
};
//
static void ShowStatus(void)
{
    wprintf(L"\n=== Microsoft Compatibility Appraiser ===\n");
    QueryTask(TASK_APPRAISER);

    wprintf(L"\n=== ProgramDataUpdater ===\n");
    QueryTask(TASK_PROGRAMDATA);
};
//
static void ShowBanner(void)
{
    wprintf(L"============================================================\n");
    wprintf(L" %s  Version %s\n", APP_NAME, APP_VERSION);
    wprintf(L" Author: %s\n", APP_AUTHOR);
    wprintf(L" %s\n", COPYRIGHT_TEXT);
    wprintf(L"============================================================\n\n");
};
//
int wmain(void)
{
    int choice = 0;
//
    SetConsoleTitleW(APP_NAME L" - CompatTelRunner Control");
    ShowBanner();
//
    if (!IsAdministrator())
    {
        wprintf(L"This program requires Administrator rights.\n");
        wprintf(L"Requesting elevation...\n");
//
        if (RelaunchAsAdministrator())
            return 0;
//
        wprintf(L"Elevation was cancelled or failed.\n");
        wprintf(L"Win32 error = %lu\n", GetLastError());
        return 1;
    };
//
    for (;;)
    {
        wprintf(L"\n============================================\n");
        wprintf(L" CompatTelRunner / Compatibility Appraiser\n");
        wprintf(L" %s v%s - %s\n", APP_NAME, APP_VERSION, APP_AUTHOR);
        wprintf(L" %s\n", COPYRIGHT_TEXT);
        wprintf(L"============================================\n");
        wprintf(L"1. Disable and stop CompatTelRunner activity\n");
        wprintf(L"2. Restore / enable scheduled tasks\n");
        wprintf(L"3. Show scheduled-task status\n");
        wprintf(L"0. Exit\n");
        wprintf(L"\nSelect: ");

        if (wscanf(L"%d", &choice) != 1)
        {
            wchar_t dummy[64];
            fgetws(dummy, 64, stdin);
            continue;
        };
//
        switch (choice)
        {
            case 1:
                DisableCompatTelemetry();
            break;
//
            case 2:
                EnableCompatTelemetry();
            break;
//
            case 3:
                ShowStatus();
            break;
//
            case 0:
                return 0;
//
            default:
                wprintf(L"Unknown selection.\n");
            break;
        };
    };
};