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
union wtime {	
	LONGLONG li;
	FILETIME ft;
};
int main(int argc, LPCSTR argv[])
{	
	STARTUPINFOA startUp;
	PROCESS_INFORMATION procInfo;
	wtime createTime, exitTime, elapsedTime;
	FILETIME kernelTime, userTime;
	SYSTEMTIME elTiSys, keTiSys, usTiSys;
	LPSTR pargv = GetCommandLineA();
	//skip first argv
	while (*pargv!= ' ') {
		pargv++;
	}
	while (*pargv == ' ') {
		pargv++;
	}
	HANDLE hProc;
	GetStartupInfoA(&startUp);
	if (!CreateProcessA(NULL, pargv, NULL, NULL, TRUE,
		NORMAL_PRIORITY_CLASS, NULL, NULL, &startUp, &procInfo)) {
		printf_s("Create Process Fail\n"); return 0;
	}
	hProc = procInfo.hProcess;
	if (WaitForSingleObject(hProc, INFINITE) != WAIT_OBJECT_0) {
		printf_s("WaitForSingleObject Fail\n"); return 0;
	}
	if (!GetProcessTimes(hProc, &createTime.ft,
		&exitTime.ft, &kernelTime, &userTime)){
      printf_s("GetProcessTimes Fail\n"); return 0;}
	elapsedTime.li = exitTime.li - createTime.li;
	FileTimeToSystemTime(&elapsedTime.ft, &elTiSys);
	FileTimeToSystemTime(&kernelTime, &keTiSys);
	FileTimeToSystemTime(&userTime, &usTiSys);
	printf("Real Time: %02d:%02d:%02d.%03d\n",
		elTiSys.wHour, elTiSys.wMinute, elTiSys.wSecond,
		elTiSys.wMilliseconds);
	printf("User Time: %02d:%02d:%02d.%03d\n",
		usTiSys.wHour, usTiSys.wMinute, usTiSys.wSecond,
		usTiSys.wMilliseconds);
	printf("Sys Time:  %02d:%02d:%02d.%03d\n",
		keTiSys.wHour, keTiSys.wMinute, keTiSys.wSecond,
		keTiSys.wMilliseconds);
	CloseHandle(procInfo.hThread); CloseHandle(procInfo.hProcess);
	return 0;
}


