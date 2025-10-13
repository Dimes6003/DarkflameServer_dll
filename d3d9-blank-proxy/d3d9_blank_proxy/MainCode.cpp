#include <Windows.h>
#include <cstdio>
#include <shlwapi.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <commdlg.h>
#include <minhook.h>
#include "MainCode.h"
#include "MinHook.h"
#include "testlog.h"
#include "Globals.h"

#pragma comment(lib, "shlwapi.lib")



DWORD g_lwoServerPID = 0;

static std::string baseDir;
static std::string lwoServerPath;

static std::string startMaximized;


// Declare original function pointer type
typedef BOOL(WINAPI* pGetOpenFileNameW)(LPOPENFILENAMEW);

// Declare global function pointer and hook function
extern pGetOpenFileNameW origGetOpenFileNameW;
BOOL WINAPI HookedGetOpenFileNameW(LPOPENFILENAMEW ofn);

void SendFileDialogData(const std::wstring& title, const std::wstring& filePath);

// Add this at the top of MainCode.cpp, before RunMainCode()
void ListenForMessageBoxPipe(HWND MainWindow);


void RunMainCode() {
    Log("started");

    if (MH_Initialize() != MH_OK) {
        Log("MinHook failed to initialize");
        return;
    }

    if (MH_CreateHookApi(L"comdlg32", "GetOpenFileNameW", &HookedGetOpenFileNameW, reinterpret_cast<LPVOID*>(&origGetOpenFileNameW)) == MH_OK) {
        MH_EnableHook(&GetOpenFileNameW);
        Log("Hooked GetOpenFileNameW");
    }
    else {
        Log("Failed to hook GetOpenFileNameW");
    }

    Log("hook installed successfully");

}


void ReadIni(HMODULE hModule)
{
    // Get DLL base directory
    char dllPath[MAX_PATH];
    GetModuleFileNameA(hModule, dllPath, MAX_PATH);
    PathRemoveFileSpecA(dllPath);
    baseDir = dllPath;

    // Build path to INI
    char iniPath[MAX_PATH];
    PathCombineA(iniPath, dllPath, "HappyFlower.ini");
    Log("Reading INI file: %s", iniPath);

    // Read INI
    std::ifstream iniFile(iniPath);
    if (!iniFile.is_open()) {
        Log("Failed to open HappyFlower.ini");
        return;
    }

    std::string line;
    while (std::getline(iniFile, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            key.erase(std::remove_if(key.begin(), key.end(), isspace), key.end());
            value.erase(std::remove_if(value.begin(), value.end(), isspace), value.end());

            if (key == "lwoServerPath") lwoServerPath = value;
        }
    }
}







typedef BOOL(WINAPI* pGetOpenFileNameW)(LPOPENFILENAMEW);
pGetOpenFileNameW origGetOpenFileNameW = nullptr;

BOOL WINAPI HookedGetOpenFileNameW(LPOPENFILENAMEW ofn)
{
    Log("GetOpenFileNameW called");

    std::wstring dialogTitle = ofn && ofn->lpstrTitle ? ofn->lpstrTitle : L"";
    std::wstring initialDir = ofn && ofn->lpstrInitialDir ? ofn->lpstrInitialDir : L"null";
    std::wstring fileFilter = ofn && ofn->lpstrFilter ? ofn->lpstrFilter : L"null";

    Log("Dialog Title: %ls", dialogTitle.empty() ? L"null" : dialogTitle.c_str());
    Log("Filter: %ls", fileFilter.c_str());
    Log("Initial Directory: %ls", initialDir.c_str());

    BOOL result = origGetOpenFileNameW(ofn);

    if (result && ofn->lpstrFile)
    {
        Log("User selected file: %ls", ofn->lpstrFile);

        std::wstring filePath = ofn->lpstrFile;
        SendFileDialogData(dialogTitle, filePath);
    }
    else
    {
        Log("Dialog was canceled or failed");
    }

    return result;
}




void SendFileDialogData(const std::wstring& title, const std::wstring& filePath)
{
    HANDLE pipe = CreateFileA(
        R"(\\.\pipe\FileDialogPipe)", // ASCII version for narrow string
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (pipe == INVALID_HANDLE_VALUE)
    {
        Log("Failed to open pipe for file dialog data. Error: %lu", GetLastError());
        return;
    }

    // Build the wide message
    std::wstring wideMessage = (title.empty() ? L"null" : title) + L"\n" + filePath + L"\n";

    // Convert to UTF-8
    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wideMessage.c_str(), -1, NULL, 0, NULL, NULL);
    std::string utf8Message(requiredSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideMessage.c_str(), -1, &utf8Message[0], requiredSize, NULL, NULL);

    // Write to pipe (skip the null terminator)
    DWORD bytesWritten = 0;
    BOOL success = WriteFile(pipe, utf8Message.c_str(), (DWORD)(utf8Message.size() - 1), &bytesWritten, NULL);

    if (!success)
    {
        Log("WriteFile to pipe failed. Error: %lu", GetLastError());
    }
    else
    {
        Log("Sent file dialog data through pipe. Message:\n%s", utf8Message.c_str());
    }

    CloseHandle(pipe);
}

