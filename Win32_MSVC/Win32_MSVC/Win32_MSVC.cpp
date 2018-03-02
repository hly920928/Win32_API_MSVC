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
	INT nThread, iThread;
	PTP_WORK *pWorkObjects;
	SRWLOCK srwl;
	unsigned int tasksPerThread, totalTasksComplete;
	workArg ** pWorkObjArgsArray;
	workArg *pThreadArg;
	TP_CALLBACK_ENVIRON cbe;  // Callback environment
// Process Input
	nThread = 4;
	tasksPerThread = 6;
	if (argc >= 4) workerDelay = 0;
	traceFlag = true;

	// Init the SRW lock 
	InitializeSRWLock(&srwl);

	pWorkObjects =(PTP_WORK*) malloc(nThread * sizeof(PTP_WORK));
	if (pWorkObjects != NULL)
		pWorkObjArgsArray = (workArg**)malloc(nThread * sizeof(workArg*));

	if (pWorkObjects == NULL || pWorkObjArgsArray == NULL) {
		printf("malloc Fail\n"); return 0;
	}
	InitializeThreadpoolEnvironment(&cbe);// Init the Threadpool
	//Create Each Work
	for (iThread = 0; iThread < nThread; iThread++) {
		pThreadArg = (pWorkObjArgsArray[iThread] =(workArg*) _aligned_malloc(sizeof(workArg), CACHE_LINE_SIZE));
		if (NULL == pThreadArg) {
			printf("malloc Fail\n"); return 0;
		}
		//Fill Arg
		pThreadArg->objectNumber = iThread;
		pThreadArg->tasksToComplete = tasksPerThread;
		pThreadArg->tasksComplete = 0;
		pThreadArg->slimRWL = srwl;
		// Create Work
		pWorkObjects[iThread] = CreateThreadpoolWork(Worker, pThreadArg, &cbe);
		if (pWorkObjects[iThread] == NULL) {
			printf("Create Work Fail\n"); return 0;
		}
		// Submit Work
		SubmitThreadpoolWork(pWorkObjects[iThread]);
	}
	for (iThread = 0; iThread < nThread; iThread++) {
		//Wait Each Work
		WaitForThreadpoolWorkCallbacks(pWorkObjects[iThread], FALSE);
		//Close Each Work
		CloseThreadpoolWork(pWorkObjects[iThread]);
	}
	printf("All Work Finish\n");
	totalTasksComplete = 0;
	for (iThread = 0; iThread < nThread; iThread++) {
		pThreadArg = pWorkObjArgsArray[iThread];
		if (traceFlag) printf("Tasks completed by thread %d: %d\n", iThread, pThreadArg->tasksComplete);
		totalTasksComplete += pThreadArg->tasksComplete;
		//_aligned_free 
		_aligned_free(pThreadArg);
	}
	if (traceFlag) printf("Total work performed: %d.\n", totalTasksComplete);
	free(pWorkObjects);
	free(pWorkObjArgsArray);
	return 0;
}

VOID CALLBACK Worker(PTP_CALLBACK_INSTANCE Instance , PVOID Context, PTP_WORK Work)
{
	workArg * wArgs;

	wArgs = (workArg*)Context;
	if (traceFlag)
		printf("Worker: %d. Thread Number: %d.\n", wArgs->objectNumber, GetCurrentThreadId());

	while (wArgs->tasksComplete < wArgs->tasksToComplete) {
		AcquireSRWLockExclusive(&(wArgs->slimRWL));
		(wArgs->tasksComplete)++;
		ReleaseSRWLockExclusive(&(wArgs->slimRWL));
	}
	return;
}
