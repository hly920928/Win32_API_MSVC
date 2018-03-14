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
	return 0;
}

DWORD QueueDestroy(QUEUE_OBJECT* q)
{
	WaitForSingleObject(q->qGuard, INFINITE);
	free(q->msgArray);
	q->msgArray = NULL;
	CloseHandle(q->qNe);
	CloseHandle(q->qNf);
	ReleaseMutex(q->qGuard);
	CloseHandle(q->qGuard);
	return 0;
}

DWORD QueueDestroyed(QUEUE_OBJECT *)
{
	return 0;
}

DWORD QueueEmpty(QUEUE_OBJECT *q)
{
	 return (q->qFirst == q->qLast);;
}

DWORD QueueFull(QUEUE_OBJECT * q)
{
	return ((q->qFirst - q->qLast) == 1 ||
		(q->qLast == q->qSize - 1 && q->qFirst == 0));
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

DWORD QueuePut(QUEUE_OBJECT * q, PVOID msg, DWORD mSize, DWORD maxWait)
{
	WaitForSingleObject(q->qGuard, INFINITE);
	if (q->msgArray == NULL) return 1;  // Queue has been destroyed
	while (QueueFull(q)) {
		SignalObjectAndWait(q->qGuard, q->qNf, INFINITE, FALSE);
		WaitForSingleObject(q->qGuard, INFINITE);
	}
	// Put the message
	QueueInsert(q, msg, mSize);
	// Signal that the queue is not empty
	PulseEvent(q->qNe);
	ReleaseMutex(q->qGuard);
	return 0;
}

DWORD QueueRemove(QUEUE_OBJECT *q, PVOID msg, DWORD mSize)
{
	char *pm;

	if (QueueEmpty(q)) return 1;//Error 
	pm = q->msgArray;
	// Remove oldest 
	memcpy(msg, pm + (q->qFirst * mSize), mSize);
	q->qFirst = ((q->qFirst + 1) % q->qSize);
	return 0;
}

DWORD QueueInsert(QUEUE_OBJECT *q, PVOID msg, DWORD mSize)
{
	char *pm;

	if (QueueFull(q)) return 1;// Error 
	pm = q->msgArray;
	// Add a new youngest 
	memcpy(pm + (q->qLast * mSize), msg, mSize);
	q->qLast = ((q->qLast + 1) % q->qSize);
	return 0;
}

void CALLBACK QueueShutDown(ULONG_PTR)
{
}
