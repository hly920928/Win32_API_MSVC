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
	InitializeSRWLock(&q->qGuard);
	InitializeConditionVariable(&q->qNe);
	InitializeConditionVariable(&q->qNf);
	return 0;
}

DWORD QueueDestroy(QUEUE_OBJECT* q)
{
	AcquireSRWLockExclusive(&q->qGuard);
	free(q->msgArray);
	q->msgArray = NULL;
	ReleaseSRWLockExclusive(&(q->qGuard));
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
	AcquireSRWLockExclusive(&q->qGuard);
	if (q->msgArray == NULL) return 1; // Queue destroyed 
	while (QueueEmpty(q)) {
		if (!SleepConditionVariableSRW(&q->qNe, &q->qGuard, INFINITE, 0)) {//release qGuard wait on qNe then Acquire qGuard
			printf("QueueGet failed. SleepConditionVariableCS.\n");
			return 0;
		}
	}
	// remove  message
	QueueRemove(q, msg, mSize);
	// Signal that the queue is not full 
	WakeConditionVariable(&q->qNf);
	ReleaseSRWLockExclusive(&q->qGuard);
	return 0;
}

DWORD QueuePut(QUEUE_OBJECT * q, PVOID msg, DWORD mSize, DWORD maxWait)
{
	AcquireSRWLockExclusive(&q->qGuard);
	if (q->msgArray == NULL) return 1;  // Queue has been destroyed
	while (QueueFull(q)) {
		if (!SleepConditionVariableSRW(&q->qNf, &q->qGuard, INFINITE, 0))//release qGuard then wait on qNf then Acquire qGuard
			printf("QueuePut failed. SleepConditionVariableCS.\n");
		return 0;
	}
	// Put the message
	QueueInsert(q, msg, mSize);
	// Signal that the queue is not empty
	WakeConditionVariable(&q->qNe);
	ReleaseSRWLockExclusive(&q->qGuard);
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
