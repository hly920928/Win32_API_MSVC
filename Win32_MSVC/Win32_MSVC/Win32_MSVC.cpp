// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <ctime>
#include "SynchObj.h"
#include "messages.h"
using namespace std;
#define DELAY_COUNT 1000
#define MAX_THREADS 1024
#define TBLOCK_SIZE 5  	//Transmitter combines
#define TBLOCK_TIMEOUT 50 //Transmiter timeout waitin
#define P2T_QLEN 10 	// Producer to Transmitter queue length 
#define T2R_QLEN 4	// Transmitter to Receiver queue length 
#define R2C_QLEN 4	//Receiver to Consumer queue length
CRITICAL_SECTION output;
DWORD WINAPI Producer(PVOID);
DWORD WINAPI Consumer(PVOID);
DWORD WINAPI Transmitter(PVOID);
DWORD WINAPI Receiver(PVOID);
void WINAPI ShutDownReceiver(DWORD n);
struct THARG {
	DWORD threadNumber;
	DWORD workGoal;
	DWORD workDone;
};

struct T2R_MSG_OBJ {
	DWORD numMessages; // Number of messages contained
	MSG_BLOCK messages[TBLOCK_SIZE];
};

struct TR_ARG {
	DWORD nProducers;  // Number of active producers 
};

QUEUE_OBJECT p2tq, t2rq, *r2cqArray;

static volatile DWORD ShutDown = 0;
static DWORD EventTimeout = 50;
DWORD trace = 0;
int main(int argc, LPCSTR argv[])
{
	//init
	InitializeCriticalSection(&output);
	DWORD tStatus, nThread, iThread, goal;
	HANDLE *producerThreadArray,
		*consumerThreadArray,
		transmitterThread,
		receiverThread;
	THARG *producerArg, *consumerArg;
	TR_ARG transmitterArg, receiverArg;
	srand((int)time(NULL));
	nThread = 10;
	receiverArg.nProducers = nThread;
	transmitterArg.nProducers = nThread;
	goal = 250;
	producerThreadArray = (HANDLE*)malloc(nThread * sizeof(HANDLE));
	producerArg = (THARG*)calloc(nThread, sizeof(THARG));
	consumerThreadArray = (HANDLE*)malloc(nThread * sizeof(HANDLE));
	consumerArg = (THARG*)calloc(nThread, sizeof(THARG));
	if (producerThreadArray == NULL || producerArg == NULL
		|| consumerThreadArray == NULL || consumerArg == NULL)
	{
		printf("Cannot allocate working memory for threads.");
		return 0;
	}
	QueueInitialize(&p2tq, sizeof(MSG_BLOCK), P2T_QLEN);
	QueueInitialize(&t2rq, sizeof(T2R_MSG_OBJ), T2R_QLEN);
	r2cqArray = (QUEUE_OBJECT*)calloc(nThread, sizeof(QUEUE_OBJECT));
	if (r2cqArray == NULL) {
		printf("Cannot allocate memory for r2c queues\n");
		return 0;
	}
	//init Producer and Consumer Thread
	for (iThread = 0; iThread < nThread; iThread++) {
		//Init consumer threads
		QueueInitialize(&r2cqArray[iThread], sizeof(MSG_BLOCK), R2C_QLEN);
		//Init the thread arg
		consumerArg[iThread].threadNumber = iThread;
		consumerArg[iThread].workGoal = goal;
		consumerArg[iThread].workDone = 0;

		consumerThreadArray[iThread] = CreateThread(NULL, 0,
			Consumer, &consumerArg[iThread], 0, NULL);
		if (consumerThreadArray[iThread] == NULL) {
			printf("Cannot create consumer thread\n");
			return 0;
		}
		producerArg[iThread].threadNumber = iThread;
		producerArg[iThread].workGoal = goal;
		producerArg[iThread].workDone = 0;
		producerThreadArray[iThread] = CreateThread(NULL, 0,
			Producer, &producerArg[iThread], 0, NULL);
		if (producerThreadArray[iThread] == NULL) {
			printf("Cannot create producer thread\n");
			return 0;
		}
	}
	//init Transmitter and Receiver Thread
	transmitterThread = CreateThread(NULL, 0, Transmitter, &transmitterArg, 0, NULL);
	if (transmitterThread == NULL) {
		printf("Cannot create tranmitter thread\n");
		return 0;
	}

	receiverThread = CreateThread(NULL, 0, Receiver, &receiverArg, 0, NULL);
	if (receiverThread == NULL) {
		printf("Cannot create receiver thread\n");
		return 0;
	}
	printf("BOSS: All threads are running\n");
	//Wait All the consumers to complete 
	for (iThread = 0; iThread < nThread; iThread++) {
		tStatus = WaitForSingleObject(consumerThreadArray[iThread], INFINITE);
		if (tStatus != 0) {
			printf("Cannot wait for consumer thread\n");
			return 0;
		}

		if (trace >= 1) {
			printf("BOSS: consumer %d consumed %d work units\n",
				iThread, consumerArg[iThread].workDone);
			return 0;
		}
	}
	printf("BOSS: All consumers have completed their work.\n");
	ShutDown = 1; //Set a shutdown flag.
				  //Wait transmitter and receiverThread
	ShutDown = 1; /* Set a shutdown flag. */

	printf("BOSS: About to cancel transmitter.\n");
	tStatus = QueueUserAPC(QueueShutDown, transmitterThread, 1);
	if (tStatus == 0) {
		printf("Failed queuing APC for transmitter\n"); return 0;
	}
	printf("BOSS: About to cancel receiver.\n");
	tStatus = QueueUserAPC(QueueShutDown, receiverThread, 2);
	if (tStatus == 0) {
		printf("Failed queuing APC for receiver\n"); return 0;
	}
	tStatus = QueueUserAPC(ShutDownReceiver, receiverThread, 2);
	if (tStatus == 0) {
		printf("Failed queuing APC for receiver\n"); return 0;
	}
	// clear up
	QueueDestroy(&p2tq);
	QueueDestroy(&t2rq);
	for (iThread = 0; iThread < nThread; iThread++) {
		QueueDestroy(&r2cqArray[iThread]);
		CloseHandle(consumerThreadArray[iThread]);
		CloseHandle(producerThreadArray[iThread]);
	}
	free(r2cqArray);
	free(producerThreadArray); free(consumerThreadArray);
	free(producerArg); free(consumerArg);
	CloseHandle(transmitterThread); CloseHandle(receiverThread);
	DeleteCriticalSection(&output);
	printf("System has finished. Shutting down\n");
	return 0;
}

DWORD WINAPI Producer(PVOID arg)
{
	THARG * parg;
	DWORD iThread;
	MSG_BLOCK msg;

	parg = (THARG *)arg;
	iThread = parg->threadNumber;

	while (parg->workDone < parg->workGoal) {
		Sleep(10);
		MessageFill(&msg, iThread, iThread, parg->workDone);
		QueuePut(&p2tq, &msg, sizeof(msg), INFINITE);

		parg->workDone++;
	}
	MessageFill(&msg, iThread, iThread, -1);
	QueuePut(&p2tq, &msg, sizeof(msg), INFINITE);
	return 0;
}

DWORD  WINAPI Consumer(PVOID arg)
{

	THARG * carg;
	DWORD tStatus, iThread;
	MSG_BLOCK msg;
	QUEUE_OBJECT *pr2cq;

	carg = (THARG *)arg;
	iThread = carg->threadNumber;
	pr2cq = &r2cqArray[iThread];

	while (carg->workDone < carg->workGoal) {
		tStatus = QueueGet(pr2cq, &msg, sizeof(msg), INFINITE);
		if (tStatus != 0) return tStatus;
		if (msg.sequence < 0) return 0;
		if (trace >= 1) printf("Message received by consumer #TID: %d. Message #: %d.\n", iThread, msg.sequence);
		if (trace >= 2) MessageDisplay(&msg);
		carg->workDone++;
	}
	return 0;
}

DWORD WINAPI Transmitter(PVOID arg)
{
	DWORD tStatus, im;
	T2R_MSG_OBJ t2r_msg = { 0 };
	TR_ARG * tArg = (TR_ARG *)arg;

	while (!ShutDown) {
		t2r_msg.numMessages = 0;
		// pack the messages for transmission to the receiver
		for (im = 0; im < TBLOCK_SIZE; im++) {
			//QueueGet could block
			tStatus = QueueGet(&p2tq, &t2r_msg.messages[im], sizeof(MSG_BLOCK), INFINITE);
			if (tStatus != 0) {
				printf("Transmitter tStatus_1 End\n");
				return tStatus;
			}
			t2r_msg.numMessages++;
			// Decrement the number of active nProducer for each negative sequence number 
			/*
			if (t2r_msg.messages[im].sequence < 0) {
			tArg->nProducers--;
			if (tArg->nProducers <= 0) break;
			}
			*/
		}

		// Transmit the block of messages 	//QueueGet could block
		tStatus = QueuePut(&t2rq, &t2r_msg, sizeof(t2r_msg), INFINITE);
		if (tStatus != 0) {
			printf("Transmitter tStatus_2 End\n");
			return tStatus;
		}
		// Terminate the transmitter if there are no active nProducers 
		if (tArg->nProducers <= 0) return 0;
	}
	printf("Transmitter End\n");
	return 0;
}

DWORD WINAPI Receiver(PVOID arg)
{
	DWORD tStatus, im, ic;
	T2R_MSG_OBJ t2r_msg;
	TR_ARG * tArg = (TR_ARG *)arg;

	while (!ShutDown) {
		//QueueGet could block
		tStatus = QueueGet(&t2rq, &t2r_msg, sizeof(t2r_msg), INFINITE);
		if (tStatus != 0) {
			printf("Receiver  tStatus_1 End\n");
			return tStatus;
		}
		//Distribute the messages to the proper consumer 
		for (im = 0; im < t2r_msg.numMessages; im++) {
			ic = t2r_msg.messages[im].destination; // Destination consumer 
												   //QueuePut could block
			tStatus = QueuePut(&r2cqArray[ic], &t2r_msg.messages[im], sizeof(MSG_BLOCK), INFINITE);
			if (tStatus != 0) {
				printf("Receiver  tStatus_2 End\n");
				return tStatus;
			}

			/*
			if (t2r_msg.messages[im].sequence < 0) {
			tArg->nProducers--;
			if (tArg->nProducers <= 0) break;
			}
			*/
		}
		//Terminate the transmitter if there are no active nProducers
		if (tArg->nProducers <= 0) return 0;
	}
	printf("Receiver End\n");
	return 0;

}
void WINAPI ShutDownReceiver(DWORD n)
{
	printf("In ShutDownReceiver. %d\n", n);
	return;
}