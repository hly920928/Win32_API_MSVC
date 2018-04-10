// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "Everything.h"
#include "ClientServer.h"	
#pragma comment(lib, "Ws2_32.lib")

struct sockaddr_in srvSAddr;	
struct sockaddr_in connectSAddr;	
WSADATA WSStartData;
static BOOL SendRequestMessage(REQUEST *, SOCKET);
static BOOL ReceiveResponseMessage(RESPONSE *, SOCKET);
enum SERVER_THREAD_STATE {
	SERVER_SLOT_FREE, SERVER_THREAD_STOPPED,
	SERVER_THREAD_RUNNING, SERVER_SLOT_INVALID
};
 struct SERVER_ARG {
	CRITICAL_SECTION threadCs;
	DWORD	number;
	SOCKET	sock;
	enum SERVER_THREAD_STATE thState;
	HANDLE	hSrvThread;
	HINSTANCE	hDll;
 };
 static DWORD WINAPI Server(PVOID);
 static DWORD WINAPI AcceptThread(PVOID);
 static BOOL  WINAPI Handler(DWORD);

 volatile static unsigned int shutFlag = 0;
 static SOCKET SrvSock = INVALID_SOCKET, connectSock = INVALID_SOCKET;
int main(int argc, LPCSTR argv[])
{
	return 0;
};

static BOOL SendRequestMessage(REQUEST * pRequest, SOCKET sd)
{
	BOOL disconnect = FALSE;
	LONG32 nRemainSend, nXfer;
	char* pBuffer;

	//request header
	nRemainSend = RQ_HEADER_LEN;
	pRequest->rqLen = (DWORD)(strlen((char*)pRequest->record) + 1);
	pBuffer = (char*)pRequest;
	while (nRemainSend > 0 && !disconnect) {
		//send does not guarantee that the entire message is sent
		nXfer = send(sd, pBuffer, nRemainSend, 0);
		if (nXfer == SOCKET_ERROR){
			printf("client send() failed\n"); return 0;
		}
		disconnect = (nXfer == 0);
		nRemainSend -= nXfer; pBuffer += nXfer;
	}
	//request record 
	nRemainSend = pRequest->rqLen;
	pBuffer = (char*)pRequest->record;
	while (nRemainSend > 0 && !disconnect) {
		nXfer = send(sd, pBuffer, nRemainSend, 0);
		if (nXfer == SOCKET_ERROR) 
		{printf("client send() failed\n"); return 0;}
		disconnect = (nXfer == 0);
		nRemainSend -= nXfer; pBuffer += nXfer;
	}
	return disconnect;
}

static BOOL ReceiveResponseMessage(RESPONSE *pResponse, SOCKET sd)
{
	BOOL disconnect = FALSE, LastRecord = FALSE;
	LONG32 nRemainRecv, nXfer;
	char* pBuffer;
	// Read the header 
	while (!LastRecord) {
		nRemainRecv = RS_HEADER_LEN; pBuffer =(char*)pResponse;
		while (nRemainRecv > 0 && !disconnect) {
			nXfer = recv(sd, pBuffer, nRemainRecv, 0);
			if (nXfer == SOCKET_ERROR) {
				printf("client response recv() failed\n"); return 0;
			}
			disconnect = (nXfer == 0);
			//Receive until read All
			nRemainRecv -= nXfer; pBuffer += nXfer;
		}
		//finish reading pResponse->rsLen
		//	Read the  record 
		nRemainRecv = pResponse->rsLen;
		// Exclude buffer overflow 
		nRemainRecv = min(nRemainRecv, MAX_RQRS_LEN);
		LastRecord = (nRemainRecv <= 1); 
		pBuffer = (char*)pResponse->record;
		while (nRemainRecv > 0 && !disconnect) {
			nXfer = recv(sd, pBuffer, nRemainRecv, 0);
			if (nXfer == SOCKET_ERROR) {
				printf("client response recv() failed\n"); return 0;
			}
			disconnect = (nXfer == 0);
			nRemainRecv -= nXfer; pBuffer += nXfer;
		}

		if (!disconnect)
			printf("%s\n", pResponse->record);
	}
	return disconnect;
}

DWORD WINAPI Server(PVOID)
{
	return 0;
}

DWORD WINAPI AcceptThread(PVOID pArg)
{
	LONG addrLen;
	SERVER_ARG * pThArg = (SERVER_ARG *)pArg;

	addrLen = sizeof(connectSAddr);
	pThArg->sock =
		accept(SrvSock, (struct sockaddr *)&connectSAddr, (int*)&addrLen);
	if (pThArg->sock == INVALID_SOCKET) {
		printf("accept: invalid socket error\n");
		return 1;
	}
	EnterCriticalSection(&(pThArg->threadCs));
	__try {
		pThArg->hSrvThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)Server, pThArg, 0, NULL);
		if (pThArg->hSrvThread == NULL) {
			printf("Failed creating server thread\n");
			return 1;
		}
		pThArg->thState = SERVER_THREAD_RUNNING;
		printf("Client accepted on slot: %d, using server thread %d.\n", pThArg->number, GetThreadId(pThArg->hSrvThread));
	}
	__finally { LeaveCriticalSection(&(pThArg->threadCs)); }
	return 0;
}

BOOL WINAPI Handler(DWORD CtrlEvent)
{

	printf("In console control handler\n");
	InterlockedIncrement(&shutFlag);
	return TRUE;
	return 0;
}
