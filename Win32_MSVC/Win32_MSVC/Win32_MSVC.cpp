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
DWORD WINAPI Producer(PVOID);
DWORD WINAPI Consumer(PVOID);
DWORD WINAPI Transmitter(PVOID);
DWORD WINAPI Receiver(PVOID);
 struct THARG {
	DWORD threadNumber;
	DWORD workGoal;   
	DWORD workDone;    
} ;

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
	
	return 0;
}

DWORD WINAPI Producer(PVOID)
{
	return 0;
}

DWORD  WINAPI Consumer(PVOID)
{
	return 0;
}

DWORD WINAPI Transmitter(PVOID)
{
	return 0;
}

DWORD WINAPI Receiver(PVOID)
{
	return 0;
}
