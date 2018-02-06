// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "Everything.h" 
#include "myHeader_V2.h" 
#include <string>
#include "jobsMngmt.h";
using namespace std;
static bool exitFlag;
BOOL WINAPI myHandler(DWORD cntrlEvent)
{
	printf("In Looping Sleep\n");
	switch (cntrlEvent) {
	case CTRL_C_EVENT:
		printf("Ctrl-C received by handler\n");
		exitFlag = TRUE;
		return TRUE;
	case CTRL_CLOSE_EVENT:
		printf("Close event received by handler\n");
		exitFlag = TRUE;
		return TRUE; 
	default:
		printf("Event: %d received by handler. Leaving in 5 seconds or less.\n", cntrlEvent);
		exitFlag = TRUE;
		return TRUE; 
	}
}

int main(int argc, LPCSTR argv[])
{
	exitFlag = false;
	if (!SetConsoleCtrlHandler(myHandler, TRUE))
	{
		printf("Error setting event handler"); return 0;
	}
	if (argc == 1) {
		printf("Please input sleepLong\n");
		return 0;
	}
	
	int sleepLong = atoi(argv[1]);
	while (!exitFlag) {
		printf("Loop Sleeping %d\n", sleepLong);
		Sleep(sleepLong);
	}
	return 0;
}


