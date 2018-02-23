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

DWORD WINAPI Consume(void* ptr)
{
	return 0;
}
void MessageFill(msgBlock *)
{
}
void MessageDisplay(msgBlock *)
{
}
;
