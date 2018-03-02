// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <cstdio>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include "Everything.h" 
#include "myHeader_V2.h" 
#include <string>
#include "jobsMngmt.h";
#include "helperFunction.h"
#define DELAY_COUNT 20
#define CACHE_LINE_SIZE 64

using namespace std;
__declspec(align(CACHE_LINE_SIZE))
 struct workArg {
	SRWLOCK slimRWL;
	int objectNumber;
	unsigned int tasksToComplete;
	unsigned int tasksComplete;
};
VOID CALLBACK Worker(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK);
int workerDelay = DELAY_COUNT;
BOOL traceFlag = FALSE;
int main(int argc, LPCSTR argv[])
{
	
	return 0;
}

VOID CALLBACK Worker(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK)
{
	return VOID();
}
