#pragma once
#include <windows.h>

void InitLog();
void CloseLog();
void Log(const char* format, ...);
