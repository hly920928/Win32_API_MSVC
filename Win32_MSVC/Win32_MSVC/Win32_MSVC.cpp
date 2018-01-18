// Win32_MSVC.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdio>
#include "Everything.h" 
#include "myHeader_V2.h" 
#include "float.h"
static BOOL exitFlag = FALSE;
BOOL WINAPI CtrlHandler(DWORD cntrlEvent)
{
	switch (cntrlEvent) {
		/* The signal timing will determine if you see the second handler message */
	case CTRL_C_EVENT:
		printf("Ctrl-C received by handler. Leaving in 5 seconds or less.\n");
		exitFlag = TRUE;
		Sleep(4000); /* Decrease this time to get a different effect */
		printf("Leaving handler in 1 second or less.\n");
		return TRUE; /* TRUE indicates that the signal was handled. */
	case CTRL_CLOSE_EVENT:
		printf("Close event received by handler. Leaving the handler in 5 seconds or less.\n");
		exitFlag = TRUE;
		Sleep(4000); /* Decrease this time to get a different effect */
		printf("Leaving handler in 1 second or less.\n");
		return TRUE; /* Try returning FALSE. Any difference? */
	default:
		printf("Event: %d received by handler. Leaving in 5 seconds or less.\n", cntrlEvent);
		exitFlag = TRUE;
		Sleep(4000); /* Decrease this time to get a different effect */
		printf("Leaving handler in 1 seconds or less.\n");
		return TRUE; /* TRUE indicates that the signal was handled. */
	}
}
int main(int argc, LPCSTR argv[])
{
	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)){
		printf("SetConsoleCtrlHandler fail/n");
	    return 0;
}
	while (!exitFlag) { 
		Sleep(4750); 
		Beep(1000, 250);
	}
	printf("Stopping the main program as requested.\n");
	return 0;
}


