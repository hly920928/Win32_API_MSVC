// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "Everything.h"
#include "ClientServer.h"	
#pragma comment(lib, "Ws2_32.lib")
#define LOCALHOST "127.0.0.1"
#define myPost 48500
struct sockaddr_in srvSAddr;	
struct sockaddr_in connectSAddr;	
WSADATA WSStartData;
static BOOL SendRequestMessage(REQUEST *, SOCKET);
static BOOL ReceiveResponseMessage(RESPONSE *, SOCKET);
static BOOL ReceiveRequestMessage(REQUEST *pRequest, SOCKET);
static BOOL SendResponseMessage(RESPONSE *pResponse, SOCKET);
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
	DWORD iThread, tStatus;
	SERVER_ARG sArgs[MAX_CLIENTS];
	HANDLE hAcceptThread = NULL;
	HINSTANCE hDll = NULL;


	// SetConsole control handler 
	if (!SetConsoleCtrlHandler(Handler, TRUE)) {
		printf("Cannot create Ctrl handler\n"); return 0;
	}

	//	Initialize the WS library
	if (WSAStartup(MAKEWORD(2, 0), &WSStartData) != 0)
	{
		printf("Cannot support sockets\n"); return 0;
	}
	//try to Open the shared command library DLL
/*

if (argc > 1) {
hDll = LoadLibraryA(argv[1]);
if (hDll == NULL) {
printf("Cannot Load DLL\n"); return 0;
}
}
*/

	// Intialize thread args 
	for (iThread = 0; iThread < MAX_CLIENTS; iThread++) {
		InitializeCriticalSection(&sArgs[iThread].threadCs);
		sArgs[iThread].number = iThread;
		sArgs[iThread].thState = SERVER_SLOT_FREE;
		sArgs[iThread].sock = 0;
		sArgs[iThread].hDll = hDll;
		sArgs[iThread].hSrvThread = NULL;
	}
	//	Follow the standard server socket/bind/listen/accept sequence 
	SrvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (SrvSock == INVALID_SOCKET) {
		printf("Failed server socket() call\n"); return 0;
	}

	//	Prepare the socket address structure

	srvSAddr.sin_family = AF_INET;
	srvSAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvSAddr.sin_port = htons(myPost);
	if (bind(SrvSock, (struct sockaddr *)&srvSAddr, sizeof(srvSAddr)) == SOCKET_ERROR)
	{
		printf("Failed server bind() call\n"); return 0;
	}
	printf("bind succ\n"); 
	if (listen(SrvSock, MAX_CLIENTS) != 0) {
		printf("Server listen() error\n"); return 0;
	}
	printf("In listening listen\n"); 

	//Main thread becomes listening/connecting/monitoring thread
	while (!shutFlag) {
		iThread = 0;
		while (!shutFlag) {
		//Continously poll the thread thState
			EnterCriticalSection(&sArgs[iThread].threadCs);
			__try {
				if (sArgs[iThread].thState == SERVER_THREAD_STOPPED) {
					// This thread stopped
					// Wait for it to stop
					tStatus = WaitForSingleObject(sArgs[iThread].hSrvThread, INFINITE);
					if (tStatus != WAIT_OBJECT_0) {
						printf("Server thread wait error\n"); return 0;
					}
					CloseHandle(sArgs[iThread].hSrvThread);
					sArgs[iThread].hSrvThread = NULL;
					sArgs[iThread].thState = SERVER_SLOT_FREE;
				}
				//Free slot identified or shut down. Use a free slot for the next connection */
				if (sArgs[iThread].thState == SERVER_SLOT_FREE || shutFlag) break;
			}
			__finally { LeaveCriticalSection(&sArgs[iThread].threadCs); }

			iThread = (iThread + 1) % MAX_CLIENTS;
			if (iThread == 0) Sleep(50);
		}
		if (shutFlag) break;
		// sArgs[iThread] == SERVER_SLOT_FREE 
		hAcceptThread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)AcceptThread, &sArgs[iThread], 0, NULL);
		if (hAcceptThread == NULL) {
			printf("Error creating AcceptThreadread.\n"); return 0;
		}
		while (!shutFlag) {
			tStatus = WaitForSingleObject(hAcceptThread, CS_TIMEOUT);
			if (tStatus == WAIT_OBJECT_0) {
			//Connection is complete 
				if (!shutFlag) {
					CloseHandle(hAcceptThread);
					hAcceptThread = NULL;
				}
				break;
			}
		}
	}  

	printf("Server shutdown in process. Wait for all server threads\n");

	while (TRUE) {
		int nRunningThreads = 0;
		for (iThread = 0; iThread < MAX_CLIENTS; iThread++) {
			EnterCriticalSection(&sArgs[iThread].threadCs);
			__try {
				if (sArgs[iThread].thState == SERVER_THREAD_RUNNING || sArgs[iThread].thState == SERVER_THREAD_STOPPED) {
					if (WaitForSingleObject(sArgs[iThread].hSrvThread, 10000) == WAIT_OBJECT_0) {
						printf("Server thread on slot %d stopped.\n", iThread);
						CloseHandle(sArgs[iThread].hSrvThread);
						sArgs[iThread].hSrvThread = NULL;
						sArgs[iThread].thState = SERVER_SLOT_INVALID;
					}
					else
						if (WaitForSingleObject(sArgs[iThread].hSrvThread, 10000) == WAIT_TIMEOUT) {
							printf("Server thread on slot %d still running.\n", iThread);
							nRunningThreads++;
						}
						else {
							printf("Error waiting on server thread in slot %d.\n", iThread);
							printf("Thread wait failure\n"); return 0;
						}

				}
			}
			__finally { LeaveCriticalSection(&sArgs[iThread].threadCs); }
		}
		if (nRunningThreads == 0) break;
	}

	if (hDll != NULL) FreeLibrary(hDll);

	//clearup
	shutdown(SrvSock, SD_BOTH);
	closesocket(SrvSock);
	WSACleanup();
	if (hAcceptThread != NULL && WaitForSingleObject(hAcceptThread, INFINITE) != WAIT_OBJECT_0) {
		printf("Failed waiting for accept thread to terminate.\n"); return 0;
	}
		
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

BOOL ReceiveRequestMessage(REQUEST * pRequest, SOCKET sd)
{
	BOOL disconnect = FALSE;
	LONG32 nRemainRecv = 0, nXfer;
	LPBYTE pBuffer;

	//Read header
	nRemainRecv = RQ_HEADER_LEN;
	pBuffer = (LPBYTE)pRequest;

	while (nRemainRecv > 0 && !disconnect) {
		nXfer = recv(sd,(char*) pBuffer, nRemainRecv, 0);
		if (nXfer == SOCKET_ERROR) {
			printf("server request recv() failed\n"); return 0;
		}
		disconnect = (nXfer == 0);
		nRemainRecv -= nXfer; pBuffer += nXfer;
	}

	//Read headerrecord 
	nRemainRecv = pRequest->rqLen;

	nRemainRecv = min(nRemainRecv, MAX_RQRS_LEN);

	pBuffer = (LPBYTE)pRequest->record;
	while (nRemainRecv > 0 && !disconnect) {
		nXfer = recv(sd, (char*)pBuffer, nRemainRecv, 0);
		if (nXfer == SOCKET_ERROR) {
			printf("server request recv() failed\n"); return 0;
		}
		disconnect = (nXfer == 0);
		nRemainRecv -= nXfer; pBuffer += nXfer;
	}

	return disconnect;
}

BOOL SendResponseMessage(RESPONSE *pResponse, SOCKET sd)
{
	BOOL disconnect = FALSE;
	LONG32 nRemainRecv = 0, nXfer, nRemainSend;
	LPBYTE pBuffer;

// send header 
	nRemainSend = RS_HEADER_LEN;
	pResponse->rsLen = (long)(strlen((char*)pResponse->record) + 1);
	pBuffer = (LPBYTE)pResponse;
	while (nRemainSend > 0 && !disconnect) {
		nXfer = send(sd, (char*)pBuffer, nRemainSend, 0);
		if (nXfer == SOCKET_ERROR) {
			printf("server send() failed\n"); return 0;
		}
		disconnect = (nXfer == 0);
		nRemainSend -= nXfer; pBuffer += nXfer;
	}
	// send remain 
	nRemainSend = pResponse->rsLen;
	pBuffer = (LPBYTE)pResponse->record;
	while (nRemainSend > 0 && !disconnect) {
		nXfer = send(sd, (char*)pBuffer, nRemainSend, 0);
		if (nXfer == SOCKET_ERROR) {
			printf("server send() failed\n"); return 0;
		}
		disconnect = (nXfer == 0);
		nRemainSend -= nXfer; pBuffer += nXfer;
	}
	return disconnect;
}

DWORD WINAPI Server(PVOID pArg)
{
	BOOL done = FALSE;
	STARTUPINFOA startInfoCh;
	SECURITY_ATTRIBUTES tempSA = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	PROCESS_INFORMATION procInfo;
	SOCKET connectSock;
	int commandLen;
	REQUEST request;	
	RESPONSE response;
	char sysCommand[MAX_RQRS_LEN];
	CHAR tempFile[100];
	HANDLE hTmpFile;
	FILE *fp = NULL;
	int(__cdecl *dl_addr)(char *, char *);
	SERVER_ARG * pThArg = (SERVER_ARG *)pArg;
	enum SERVER_THREAD_STATE threadState;

	GetStartupInfoA(&startInfoCh);

	connectSock = pThArg->sock;
	//Create a temp file name 
	tempFile[sizeof(tempFile) / sizeof(CHAR) - 1] = '\0';
	sprintf_s(tempFile, sizeof(tempFile) / sizeof(CHAR) - 1, "ServerTemp%d.tmp", pThArg->number);
	// Main Server Loop
	while (!done && !shutFlag) { 	
		done = ReceiveRequestMessage(&request, connectSock);

		request.record[sizeof(request.record) - 1] = '\0';
		commandLen = (int)(strcspn((CHAR*)request.record, "\n\t"));
		memcpy(sysCommand, request.record, commandLen);
		sysCommand[commandLen] = '\0';
		printf("Command received on server slot %d: %s\n", pThArg->number, sysCommand);

		/* Restest shutFlag, as it can be set from the console control handler. */
		done = done || (strcmp((CHAR*)request.record, "$Quit") == 0) || shutFlag;
		if (done) continue;

		/* Open the temporary results file. */
		hTmpFile = CreateFileA(tempFile, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, &tempSA,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hTmpFile == INVALID_HANDLE_VALUE) {
			printf("Cannot create temp file\n"); return 0;
		}

		//using DLL
		dl_addr = NULL; 
		if (pThArg->hDll != NULL) { 
			char commandName[256] = "";
			int commandNameLength = (int)(strcspn(sysCommand, " "));
			strncpy_s(commandName, sizeof(commandName), sysCommand, min(commandNameLength, sizeof(commandName) - 1));
			dl_addr = (int(*)(char *, char *))GetProcAddress(pThArg->hDll, commandName);
			if (dl_addr != NULL) __try {
				(*dl_addr)((char*)request.record, tempFile);
			}
			__except (EXCEPTION_EXECUTE_HANDLER) { /* Exception in the DLL */
				printf("Unhandled Exception in DLL. Terminate server. There may be orphaned processes.\n");
				return 1;
			}
		}
		//using process
		if (dl_addr == NULL) {
			startInfoCh.hStdOutput = hTmpFile;
			startInfoCh.hStdError = hTmpFile;
			startInfoCh.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
			startInfoCh.dwFlags = STARTF_USESTDHANDLES;
			if (!CreateProcessA(NULL, (char*)request.record, NULL,
				NULL, TRUE, /* Inherit handles. */
				0, NULL, NULL, &startInfoCh, &procInfo)) {
				char error[] = "ERR: Cannot create process.\n";
				DWORD writted = -1;
				WriteFile(hTmpFile, error, sizeof(error), &writted, NULL);
				procInfo.hProcess = NULL;
			}
			CloseHandle(hTmpFile);
			if (procInfo.hProcess != NULL) {
				CloseHandle(procInfo.hThread);
				WaitForSingleObject(procInfo.hProcess, INFINITE);
				CloseHandle(procInfo.hProcess);
			}
		}


		//send to client
		if (fopen_s(&fp, tempFile, "r") == 0) {
			{
				response.rsLen = MAX_RQRS_LEN;
				while ((fgets((char*)response.record, MAX_RQRS_LEN, fp) != NULL))
					SendResponseMessage(&response, connectSock);
			}
			/* Send a zero length message. Messages are 8-bit characters, not UNICODE. */
			response.record[0] = '\0';
			SendResponseMessage(&response, connectSock);
			fclose(fp); fp = NULL;
			DeleteFileA(tempFile);
		}
		else {
			printf("Failed to open temp file with command results\n"); return 0;
		}

	}   // main loop End

		//clear up
	printf("Shuting down server thread number %d\n", pThArg->number);
	/* Redundant shutdown. There are no further attempts to send or receive */
	shutdown(connectSock, SD_BOTH);
	closesocket(connectSock);

	EnterCriticalSection(&(pThArg->threadCs));
	__try {
		threadState = pThArg->thState = SERVER_THREAD_STOPPED;
	}
	__finally { LeaveCriticalSection(&(pThArg->threadCs)); }

	return threadState;
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
