#pragma once
#include "Everything.h"
struct thresholdBarrier { 	
	HANDLE bGuard;	// mutex 
	HANDLE bEvent;//auto reset event
	int bCount;		//number of threads that have reached the Barrier 
	int bThreshold;	// Barrier threshold 
};
typedef  thresholdBarrier* pThresholdBarrier;

int CreateThresholdBarrier(pThresholdBarrier* , DWORD /* threshold */);
int WaitThresholdBarrier(pThresholdBarrier);
int CloseThresholdBarrier(pThresholdBarrier);

#define SYNCH_OBJ_NOMEM 1  /* Unable to allocate resources */
#define SYNCH_OBJ_CREATE_FAILURE 2
#define SYNCH_OBJ_BUSY 3  /* Object is in use and cannot be closed */