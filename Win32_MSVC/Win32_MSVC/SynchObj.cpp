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
	ReleaseMutex(thb->bGuard);
	if (thb->bCount < thb->bThreshold){ //thread too little	
	//SignalObjectAndWait(thb->bGuard, thb->bEvent, INFINITE, FALSE);
	//WaitForSingleObject(thb->bGuard, INFINITE);
	WaitForSingleObject(thb->bEvent, INFINITE);
    }else {
		if (thb->bCount == thb->bThreshold)
		{
			printf("Set Event\n");
			SetEvent(thb->bEvent);
		}
	}
	int t = 666;
	//ReleaseMutex(thb->bGuard);
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

DWORD QueueInitialize(QUEUE_OBJECT *q, DWORD mSize, DWORD nMsgs)
{

	if ((q->msgArray =(char*) calloc(nMsgs, mSize)) == NULL) return 1;
	q->qFirst = q->qLast = 0;
	q->qSize = nMsgs;
	q->qGuard = CreateMutexA(NULL, FALSE, NULL);
	q->qNe = CreateEventA(NULL, TRUE, FALSE, NULL);
	q->qNf = CreateEventA(NULL, TRUE, FALSE, NULL);
	return 0; /* No error */
}

DWORD QueueDestroy(QUEUE_OBJECT *)
{
	return 0;
}

DWORD QueueDestroyed(QUEUE_OBJECT *)
{
	return 0;
}

DWORD QueueEmpty(QUEUE_OBJECT *)
{
	return 0;
}

DWORD QueueFull(QUEUE_OBJECT *)
{
	return 0;
}

DWORD QueueGet(QUEUE_OBJECT *q, PVOID msg, DWORD mSize, DWORD maxWait)
{
	WaitForSingleObject(q->qGuard, INFINITE);
	if (q->msgArray == NULL) return 1; // Queue destroyed 
	while (QueueEmpty(q)) {
		SignalObjectAndWait(q->qGuard, q->qNe, INFINITE, FALSE);
		WaitForSingleObject(q->qGuard, INFINITE);
	}
	// remove  message
	QueueRemove(q, msg, mSize);
	// Signal that the queue is not full 
	PulseEvent(q->qNf);
	ReleaseMutex(q->qGuard);

	return 0;
	return 0;
}

DWORD QueuePut(QUEUE_OBJECT *, PVOID, DWORD, DWORD)
{
	return 0;
}

DWORD QueueRemove(QUEUE_OBJECT *, PVOID, DWORD)
{
	return 0;
}

DWORD QueueInsert(QUEUE_OBJECT *, PVOID, DWORD)
{
	return 0;
}

void CALLBACK QueueShutDown(ULONG_PTR)
{
}
