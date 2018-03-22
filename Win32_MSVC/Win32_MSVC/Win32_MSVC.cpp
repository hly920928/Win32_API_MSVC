// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <ctime>
#include "SynchObj.h"
#include <iostream>
#include <string>
#include "messages.h"
using namespace std;
void randStr(char* buffer) {
	for (int i = 0; i < 5; i++) {
		buffer[i] = 'a'+rand() % 26;
	}
	buffer[5] = '\n';
}
int main(int argc, LPCSTR argv[])
{
	SECURITY_ATTRIBUTES pipeSA = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	PROCESS_INFORMATION procInfo1, procInfo2;
	STARTUPINFOA startInfoCh1, startInfoCh2;
	GetStartupInfoA(&startInfoCh1);
	GetStartupInfoA(&startInfoCh2);
	HANDLE hReadPipe, hWritePipe;
	if (!CreatePipe(&hReadPipe, &hWritePipe, &pipeSA, 0)) {
		printf("Pipe Create Fail\n");
		return 0;
	}
	//Process 1
	startInfoCh1.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	startInfoCh1.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	startInfoCh1.hStdOutput = hWritePipe;
	startInfoCh1.dwFlags = STARTF_USESTDHANDLES;
	if (!CreateProcessA(NULL, "ReadP_0", NULL, NULL,
		TRUE,			/* Inherit handles. */
		0, NULL, NULL, &startInfoCh1, &procInfo1)) {
		printf("Process 1 Create Fail\n");
		return 0;
	}
	CloseHandle(procInfo1.hThread);
	CloseHandle(hWritePipe);

	//Process 2
	startInfoCh2.hStdInput = hReadPipe;
	startInfoCh2.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	startInfoCh2.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	startInfoCh2.dwFlags = STARTF_USESTDHANDLES;
	if (!CreateProcessA(NULL, "ReadP", NULL, NULL,
		TRUE,			/* Inherit handles. */
		0, NULL, NULL, &startInfoCh2, &procInfo2)) {
		printf("Process 2 Create Fail\n");
		return 0;
	}
	CloseHandle(procInfo2.hThread);
	CloseHandle(hReadPipe);

	WaitForSingleObject(procInfo1.hProcess, INFINITE);
	WaitForSingleObject(procInfo2.hProcess, INFINITE);
	CloseHandle(procInfo1.hProcess);
	CloseHandle(procInfo2.hProcess);
	return 0;
};

