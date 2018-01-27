// Win32_MSVC.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdio>
#include <iostream>
#include "Everything.h" 
#include "myHeader_V2.h" 
typedef void(*ptr1)(char*) ;
int main(int argc, LPCSTR argv[])
{	
	DWORD pid =GetCurrentProcessId();
	printf("PID is %d ", pid);
	if (argc == 1) {
		printf("No second Command line argv\n");
		ExitProcess(0);
		return 0;
   }
	printf("Caller is %s\n",argv[1]);
	ExitProcess(1);
	return 0;
}


