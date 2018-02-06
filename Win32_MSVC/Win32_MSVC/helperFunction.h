#pragma once
#include "Everything.h"
LPSTR SkipArg(LPSTR cLine, int argn, int argc, LPSTR argv[]);
DWORD Options(int argc, LPSTR argv[], LPCSTR OptStr, ...);
void GetArgs(LPCSTR Command, DWORD* pArgc, LPSTR argstr[]);
