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
#define DATA_SIZE 256
using namespace std;
struct msgBlock {
	HANDLE mGuard;	//Mutex HANDLE
	HANDLE	mReady; // Event  HANDLE
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
	DWORD status;
	HANDLE hProduce, hConsume;
	// Init  Sync Object
	gobalBlock.mGuard = CreateMutexA(NULL, FALSE, NULL);
	gobalBlock.mReady = CreateEventA(NULL, FALSE, FALSE, NULL);

	hProduce = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)Produce, NULL, 0, NULL);
	if (hProduce == NULL)
		printf("Cannot create Producer thread\n");
	hConsume = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)Consume, NULL, 0, NULL);
	if (hConsume == NULL)
		printf("Cannot create Consumer thread\n");

	status = WaitForSingleObject(hConsume, INFINITE);
	if (status != WAIT_OBJECT_0)
		printf("Failed waiting for Consumer thread\n");
	status = WaitForSingleObject(hProduce, INFINITE);
	if (status != WAIT_OBJECT_0)
		printf("Failed waiting for Producer thread\n");
	// Delete  Sync Object
	CloseHandle(gobalBlock.mGuard);
	CloseHandle(gobalBlock.mReady);;

	printf("Producer and Consumer threads have terminated\n");
	printf("Messages Produced: %d, Consumed: %d, Lost: %d.\n",
		gobalBlock.mSequence, gobalBlock.nCons, gobalBlock.mSequence - gobalBlock.nCons);
	return 0;
}

DWORD WINAPI Produce(void* args)
{
	srand((DWORD)time(NULL));
	auto& local = gobalBlock;
	while (!gobalBlock.fStop) {
		Sleep(1500+rand() / 100);
		WaitForSingleObject(gobalBlock.mGuard,INFINITE);//Wait Mutex
		__try {
			if (!gobalBlock.fStop) {
				gobalBlock.fReady = 0;
				MessageFill(&gobalBlock);
				gobalBlock.fReady = 1;
				InterlockedIncrement(&gobalBlock.mSequence);
				SetEvent(gobalBlock.mReady);//Set Event
			}
		}
		__finally { ReleaseMutex(gobalBlock.mGuard); }//Release Mutex
	}
	return 0;
};

DWORD WINAPI Consume(void* args)
{
	CHAR command, extra;
	auto& local = gobalBlock;
	while (!gobalBlock.fStop) {
		printf("Enter 'c' for Consume; 's' to stop: ");
		scanf("%c%c", &command, &extra);
		if (command == 's') {
			gobalBlock.fStop = 1;
		}
		else if (command == 'c') {
			WaitForSingleObject(gobalBlock.mReady, INFINITE);//Wait Event
			WaitForSingleObject(gobalBlock.mGuard, INFINITE);//Wait Mutex In this ORDER
			__try {
				if (gobalBlock.fReady == 0) {
					printf("No new messages. Try again later\n");
					_leave;
				}else {
					MessageDisplay(&gobalBlock);
					gobalBlock.nLost = gobalBlock.mSequence - gobalBlock.nCons + 1;
					gobalBlock.fReady = 0; /* No new messages are ready */
					InterlockedIncrement(&gobalBlock.nCons);
				}
			}
			__finally { ReleaseMutex(gobalBlock.mGuard); }//Release Mutex
		}
		else {
			printf("Illegal command. Try again.\n");
		}
	}
	return 0;
}
void MessageFill(msgBlock * msgBlock)
{
	DWORD i;

	msgBlock->mChecksum = 0;
	for (i = 0; i < DATA_SIZE; i++) {
		msgBlock->mData[i] = rand();
		msgBlock->mChecksum ^= msgBlock->mData[i];//produce Checksum  
	}
	 time(&msgBlock->mTimestamp);
	return;
}
void MessageDisplay(msgBlock * msgBlock )
{
	DWORD i, tcheck = 0;

	for (i = 0; i < DATA_SIZE; i++)
		tcheck ^= msgBlock->mData[i];//produce Checksum  
	printf("Message number %d generated at: %s",
		msgBlock->mSequence, ctime(&msgBlock->mTimestamp));
	printf("First and last entries: %x %x\n",
		msgBlock->mData[0], msgBlock->mData[DATA_SIZE - 1]);
	if (tcheck == msgBlock->mChecksum) //Check Checksum
		printf("GOOD ->mChecksum was validated.\n");
	else
		printf("BAD  ->mChecksum failed. message was corrupted\n");
	printf("\n");
	return;
}
;
