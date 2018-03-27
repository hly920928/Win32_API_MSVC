// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <ctime>
#include "SynchObj.h"
#include <iostream>
#include <string>
#include "messages.h"
#include  "ClientServer.h"
using namespace std;
 struct THREAD_ARG {			
	HANDLE hNamedPipe;		
	DWORD threadNumber;
	TCHAR tempFileName[MAX_PATH];
 };
 typedef THREAD_ARG *LPTHREAD_ARG;
 volatile static unsigned int shutDown = 0;
 static DWORD WINAPI Server(LPTHREAD_ARG);
 static DWORD WINAPI Connect(LPTHREAD_ARG);
 static DWORD WINAPI ServerBroadcast(LPLONG);
 static BOOL  WINAPI Handler(DWORD);
 static CHAR shutRequest[] = "$ShutDownServer";
 static THREAD_ARG threadArgs[MAX_CLIENTS];
int main(int argc, LPCSTR argv[])
{
	
	return 0;
};

DWORD WINAPI Server(LPTHREAD_ARG)
{
	return 0;
}

DWORD WINAPI Connect(LPTHREAD_ARG)
{
	return 0;
}

DWORD WINAPI ServerBroadcast(LPLONG)
{
	return 0;
}

BOOL WINAPI Handler(DWORD CtrlEvent)
{
	printf("In console control handler\n");
	InterlockedIncrement(&shutDown);
	return TRUE;
}
