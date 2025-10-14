#include "testlog.h"
#include "MainCode.h"
#include <cstdio>
#include <cstdarg>
#include <windows.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

static FILE* logFile = nullptr;



void InitLog() {
    if (logFile || !debugMode) return; // Already open or disabled

    char dllPath[MAX_PATH];
    if (GetModuleFileNameA((HMODULE)&__ImageBase, dllPath, MAX_PATH)) {
        // Remove filename, keep directory only
        char* lastSlash = strrchr(dllPath, '\\');
        if (lastSlash) *(lastSlash + 1) = '\0';
        strcat_s(dllPath, MAX_PATH, "testlog.txt");

        // Delete the old log file if it exists
        DeleteFileA(dllPath);

        // Open a new log file in write mode (not append)
        fopen_s(&logFile, dllPath, "w");
        if (!logFile) {
            MessageBoxA(NULL, "Failed to open testlog.txt for logging", "Logger", MB_ICONERROR);
        }
    }
}


void CloseLog() {
    if (logFile && debugMode) {
        fclose(logFile);
        logFile = nullptr;
    }
}

void Log(const char* format, ...) {
    if (!logFile || !debugMode) return;

    va_list args;
    va_start(args, format);
    vfprintf(logFile, format, args);
    fprintf(logFile, "\n");
    fflush(logFile);
    va_end(args);
}
