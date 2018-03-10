#include "SynchObj.h"

int CreateThresholdBarriers(pThresholdBarrier* pThb, int bValue)
{
	pThresholdBarrier objThb;
	/* Initialize a oBarrier object */
	objThb = (pThresholdBarrier*)malloc(sizeof(thresholdBarrier));
	if (objThb == NULL) return SYNCH_OBJ_NOMEM;

	objThb->bGuard = CreateMutex(NULL, FALSE, NULL);
	if (objThb->bGuard == NULL) return SYNCH_OBJ_CREATE_FAILURE;

	/* Manual reset event */
	objThb->bEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (objThb->bEvent == NULL) return SYNCH_OBJ_CREATE_FAILURE;

	objThb->bThreshold = bValue;
	objThb->bCount = 0;

	*pThb = objThb;

	return 0;
}

int WaitOnThresholdBarriers(pThresholdBarrier)
{
	return 0;
}

int CloseThresholdBarriers(pThresholdBarrier)
{
	return 0;
}
