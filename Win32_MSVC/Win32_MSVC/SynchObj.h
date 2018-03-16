#pragma once
#include "stdafx.h"
#include "Everything.h"
#define SYNCH_OBJ_NOMEM 1  
#define SYNCH_OBJ_CREATE_FAILURE 2
#define SYNCH_OBJ_BUSY 3 
struct thresholdBarrier {
	HANDLE bGuard;	// mutex 
	HANDLE bEvent;//auto reset event
	int bCount;		//number of threads that have reached the Barrier 
	int bThreshold;	// Barrier threshold 
};
typedef  thresholdBarrier* pThresholdBarrier;

int CreateThresholdBarriers(pThresholdBarrier*, DWORD);
int WaitOnThresholdBarriers(pThresholdBarrier);
int CloseThresholdBarriers(pThresholdBarrier);
//Using CONDITION VARIABLE and Slim Reader/Writer Locks
struct QUEUE_OBJECT {
	SRWLOCK	qGuard;//SRWLOCK
	CONDITION_VARIABLE qNe;	// CONDITION_VARIABLE:Queue is not empty
	CONDITION_VARIABLE	qNf;	//CONDITION_VARIABLE: Queue is not full			
	DWORD qSize;	// Queue max size		
	DWORD qFirst;	// Index of oldest message
	DWORD qLast;	//Index of youngest msg	
	char	*msgArray;	// array of qSize messages	
};
// Queue management functions 
DWORD QueueInitialize(QUEUE_OBJECT *, DWORD, DWORD);
DWORD QueueDestroy(QUEUE_OBJECT *);
DWORD QueueDestroyed(QUEUE_OBJECT *);
DWORD QueueEmpty(QUEUE_OBJECT *);
DWORD QueueFull(QUEUE_OBJECT *);
DWORD QueueGet(QUEUE_OBJECT *, PVOID, DWORD, DWORD);
DWORD QueuePut(QUEUE_OBJECT *, PVOID, DWORD, DWORD);
DWORD QueueRemove(QUEUE_OBJECT *, PVOID, DWORD);
DWORD QueueInsert(QUEUE_OBJECT *, PVOID, DWORD);
void CALLBACK QueueShutDown(ULONG_PTR);

