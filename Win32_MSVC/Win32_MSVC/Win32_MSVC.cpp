// Win32_MSVC.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "Everything.h" 
#include "myHeader_V2.h" 
#include <string>
typedef void(*ptr1)(char*) ;
using namespace std;
struct PROCFILE {
	CHAR tempFile[100];
};
int main(int argc, LPCSTR argv[])
{	
	HANDLE hTempFile;
	SECURITY_ATTRIBUTES stdOutSA =
	{ sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	char commandLine[MAX_PATH + 100];
	STARTUPINFOA startUpSearch, startUp;
	PROCESS_INFORMATION processInfo;
	DWORD exitCode, dwCreationFlags = 0;
	int iProc = 5;
	GetStartupInfoA(&startUpSearch);
	GetStartupInfoA(&startUp);
	std::vector<PROCFILE>procFile; procFile.resize(iProc - 1);
	std::vector<HANDLE>hProc; hProc.resize(iProc - 1);
	// in for
	sprintf(commandLine, "printOut %s", "t2222");
	//printf("%s", commandLine);
	if (GetTempFileNameA(".", "gtm", 0, procFile[0].tempFile) == 0)
	{
		printf("Temp File Name fail"); return 0;
	}
	const char tfile[] = "myTempFile.txt";
	hTempFile =
		CreateFileA(tfile,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, &stdOutSA,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hTempFile == INVALID_HANDLE_VALUE) {
		printf("Create Temp File fail"); return 0;
	}
	startUpSearch.dwFlags = STARTF_USESTDHANDLES;
	startUpSearch.hStdOutput = hTempFile;
	startUpSearch.hStdError = hTempFile;
	startUpSearch.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	if (!CreateProcessA(NULL, commandLine, NULL, NULL,
		TRUE, dwCreationFlags, NULL, NULL, &startUpSearch, &processInfo)) {
		printf("Create Process fail"); return 0;
	}
	hProc[0] = processInfo.hProcess;
	CloseHandle(hTempFile); CloseHandle(processInfo.hThread);
	WaitForMultipleObjects(argc - 1, hProc.data(), TRUE, INFINITE);

	if (GetExitCodeProcess(hProc[0], &exitCode) && exitCode == 1) {
		fstream fs;
		fs.open("myTempFile.txt");
		string t;
		fs >> t;
		cout << t << endl;
		fs.close();
	}
	CloseHandle(hProc[0]);
	if (!DeleteFileA(tfile)) {
	printf("Delete File fail\0"); return 0;}
	return 0;
}


