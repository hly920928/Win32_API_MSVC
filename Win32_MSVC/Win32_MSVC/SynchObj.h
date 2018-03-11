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

