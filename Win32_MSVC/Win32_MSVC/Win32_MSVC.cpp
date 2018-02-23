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
#include "helperFunction.h"
#define DATA_SIZE 256
using namespace std;
struct msgBlock {
	CRITICAL_SECTION mGuard;	
	DWORD fReady, fStop;
	volatile DWORD nCons, mSequence; 
	DWORD nLost;
	time_t mTimestamp;
	DWORD mChecksum; 
	DWORD mData[DATA_SIZE]; 
};
DWORD WINAPI Produce(void*);
DWORD WINAPI Consume(void*);
void MessageFill(msgBlock *);
void MessageDisplay(msgBlock *);
msgBlock gobalBlock = { 0, 0, 0, 0, 0 };
int main(int argc, LPCSTR argv[])
{
	
  
	return 0;
}

DWORD WINAPI Produce(void* args)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	srand(st.wMilliseconds);
	while (!gobalBlock.fStop) {
		Sleep(rand() / 100);
		EnterCriticalSection(&gobalBlock.mGuard);//Enter CS
		__try {
			if (!gobalBlock.fStop) {
				gobalBlock.fReady = 0;
				MessageFill(&gobalBlock);
				gobalBlock.fReady = 1;
				InterlockedIncrement(&gobalBlock.mSequence);
			}
		}
		__finally { LeaveCriticalSection(&gobalBlock.mGuard); }//Leave CS
	}
	return 0;
};

DWORD WINAPI Consume(void* args)
{
	CHAR command, extra;
	while (!gobalBlock.fStop) {
		printf("Enter 'c' for Consume; 's' to stop: ");
		scanf("%c%c", &command, &extra);
		if (command == 's') {
			gobalBlock.fStop = 1;
		}
		else if (command == 'c') {
			EnterCriticalSection(&gobalBlock.mGuard);
			__try {
				if (gobalBlock.fReady == 0)
					printf("No new messages. Try again later\n");
				else {
					MessageDisplay(&gobalBlock);
					gobalBlock.nLost = gobalBlock.mSequence - gobalBlock.nCons + 1;
					gobalBlock.fReady = 0; /* No new messages are ready */
					InterlockedIncrement(&gobalBlock.nCons);
				}
			}
			__finally { LeaveCriticalSection(&gobalBlock.mGuard); }
		}
		else {
			printf("Illegal command. Try again.\n");
		}
	}
	return 0;
}
void MessageFill(msgBlock *)
{
}
void MessageDisplay(msgBlock *)
{
}
;
