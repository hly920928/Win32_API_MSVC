// DLLTest.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <cstdio>
extern "C" _declspec(dllexport) void printOut_V1(char* s) {
	printf_s("In printOut_V1 Dll\n");
	printf_s("%s\n", s);
}

extern "C" _declspec(dllexport) void printOut_V2(char* s) {
	printf_s("In printOut_V2 Dll\n");
	printf_s("%s\n", s);
}

