#include "stdafx.h"
#include "SynchObj.h"

int CreateThresholdBarriers(pThresholdBarrier* pThb, DWORD bValue)
{
	pThresholdBarrier objThb;
	//Init
	objThb = (pThresholdBarrier)malloc(sizeof(thresholdBarrier));
	if (objThb == NULL) return SYNCH_OBJ_NOMEM;
	objThb->bGuard = CreateMutex(NULL, FALSE, NULL);
	if (objThb->bGuard == NULL)return SYNCH_OBJ_CREATE_FAILURE;

	//Manual reset event 
	objThb->bEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
	if (objThb->bEvent == NULL) return SYNCH_OBJ_CREATE_FAILURE;

	objThb->bThreshold = bValue;
	objThb->bCount = 0;

	*pThb = objThb;

	return 0;
}

int WaitOnThresholdBarriers(pThresholdBarrier thb)
{

	WaitForSingleObject(thb->bGuard, INFINITE);//wait mutex
	thb->bCount++;  //threadCount ++
	while (thb->bCount < thb->bThreshold) {//thread too little
		SignalObjectAndWait(thb->bGuard, thb->bEvent, INFINITE, FALSE);
		WaitForSingleObject(thb->bGuard, INFINITE);
	}
	SetEvent(thb->bEvent);
	ReleaseMutex(thb->bGuard);
	return 0;
}

int CloseThresholdBarriers(pThresholdBarrier thb)
{
	ReleaseMutex(thb->bGuard);
	CloseHandle(thb->bEvent);
	CloseHandle(thb->bGuard);
	free(thb);
	return 0;
}
