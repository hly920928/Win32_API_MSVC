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
#include "SynchObj.h"
#define DELAY_COUNT 20
#define CACHE_LINE_SIZE 64

using namespace std;
struct threadTestArg {
	int id;
	pThresholdBarrier pTb;
	threadTestArg(int i =-1, pThresholdBarrier t=nullptr) :id(i),pTb(t) {};
};
CRITICAL_SECTION output;
DWORD WINAPI threadTest(void *arg);
int main(int argc, LPCSTR argv[])
{
	InitializeCriticalSection(&output);
	pThresholdBarrier pbarrier;

	CreateThresholdBarriers(&pbarrier, 5);
	

	HANDLE hd[200];
	threadTestArg arg[20];
	for (int i = 0; i < 10; i++) {
		arg[i]=threadTestArg(i, pbarrier);
		hd[i]=CreateThread(NULL,NULL, threadTest,&arg[i],NULL,NULL);
	}	 
	WaitForMultipleObjects(10, hd, TRUE, INFINITE);
	//clear up
	CloseThresholdBarriers(pbarrier);
	DeleteCriticalSection(&output);
	return 0;
}

DWORD WINAPI threadTest(void * arg)
{
	threadTestArg* argm = (threadTestArg*) arg;
	EnterCriticalSection(&output);
	printf("Enter Thread Id = %d\n", argm->id);
	LeaveCriticalSection(&output);
	WaitOnThresholdBarriers(argm->pTb);
	EnterCriticalSection(&output);
	printf("_Leave Thread Id = %d\n", argm->id);
	LeaveCriticalSection(&output);
	return 0;
}
