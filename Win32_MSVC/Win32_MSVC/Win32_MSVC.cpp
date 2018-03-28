// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <ctime>
#include "SynchObj.h"
#include <iostream>
#include <string>
#include "messages.h"
#include  "ClientServer.h"
using namespace std;
 struct THREAD_ARG {			
	HANDLE hNamedPipe;		
	DWORD threadNumber;
	CHAR tempFileName[MAX_PATH];
 };
 typedef THREAD_ARG *LPTHREAD_ARG;
 volatile static unsigned int shutDown = 0;
 static DWORD WINAPI Server(LPTHREAD_ARG);
 static DWORD WINAPI Connect(LPTHREAD_ARG);
 static DWORD WINAPI ServerBroadcast(LPLONG);
 static BOOL  WINAPI Handler(DWORD);
 static CHAR shutRequest[] = "$ShutDownServer";
 static THREAD_ARG threadArgs[MAX_CLIENTS];
int main(int argc, LPCSTR argv[])
{
	

	HANDLE hNp, hMonitor, hSrvrThread[MAX_CLIENTS];
	DWORD iNp, monitorId, threadId;
	DWORD AceMasks[] =
	{ STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0X1FF, 0, 0 };
	LPSECURITY_ATTRIBUTES pNPSA = NULL;

	if (!SetConsoleCtrlHandler(Handler, TRUE)) {
		printf("Cannot create Ctrl handler/n");
		return 0;
	}

	hMonitor = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)ServerBroadcast, NULL, 0, (unsigned int*)&monitorId);

	/*	Create a pipe instance for every server thread.
	*	Create a temp file name for each thread.
	*	Create a thread to service that pipe. */

	for (iNp = 0; iNp < MAX_CLIENTS; iNp++) {
		hNp = CreateNamedPipeA(SERVER_PIPE, PIPE_ACCESS_DUPLEX,
			PIPE_READMODE_MESSAGE | PIPE_TYPE_MESSAGE | PIPE_WAIT,
			MAX_CLIENTS, 0, 0, INFINITE, pNPSA);

		if (hNp == INVALID_HANDLE_VALUE)
		{
			printf("Failure to open named pipe.\n");	return 0;
		}
		threadArgs[iNp].hNamedPipe = hNp;
		threadArgs[iNp].threadNumber = iNp;
		GetTempFileNameA(".", "CLP", 0, threadArgs[iNp].tempFileName);
		hSrvrThread[iNp] = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)Server,
			&threadArgs[iNp], 0, (unsigned int*)&threadId);
		if (hSrvrThread[iNp] == NULL) {
			printf("Failure to create server thread.\n");	return 0;
		}
	}

	// Wait 
	printf("On Waiting All Server worker threads have shut down.\n");
	WaitForMultipleObjects(MAX_CLIENTS, hSrvrThread, TRUE, INFINITE);
    printf("All Server worker threads have shut down.\n");

	WaitForSingleObject(hMonitor, INFINITE);
	printf("Monitor thread has shut down.\n");
	//clean up
	CloseHandle(hMonitor);
	for (iNp = 0; iNp < MAX_CLIENTS; iNp++) {
		
		CloseHandle(hSrvrThread[iNp]);
		DeleteFileA(threadArgs[iNp].tempFileName);
	}

	printf("Server process will exit.\n");
	return 0;
};

DWORD WINAPI Server(LPTHREAD_ARG pThArg)
{

	HANDLE hNamedPipe, hTmpFile = INVALID_HANDLE_VALUE, hConTh = NULL, hClient;
	DWORD nXfer, conThStatus, clientProcessId;
	STARTUPINFOA startInfoCh;
	SECURITY_ATTRIBUTES tempSA = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	PROCESS_INFORMATION procInfo;
	FILE *fp;
	REQUEST request;
	RESPONSE response;
	CHAR clientName[256];

	GetStartupInfoA(&startInfoCh);
	hNamedPipe = pThArg->hNamedPipe;

	while (!shutDown) {

		hConTh = (HANDLE)_beginthreadex(NULL, 0,(_beginthreadex_proc_type) Connect, pThArg, 0, NULL);
		if (hConTh == NULL) {
			printf("Cannot create connect thread\n");
			_endthreadex(2);
		}
		//look at shutDown and wait until connect
		while (!shutDown && WaitForSingleObject(hConTh, CS_TIMEOUT) == WAIT_TIMEOUT)
		{ // Empty loop
		};
		if (shutDown) printf("Thread %d received shut down\n", pThArg->threadNumber);
		if (shutDown) continue;	

		CloseHandle(hConTh); hConTh = NULL;

		//Get Client Info
		if (!GetNamedPipeClientComputerNameA(pThArg->hNamedPipe, clientName, sizeof(clientName))) {
			strcpy_s(clientName, sizeof(clientName) / sizeof(CHAR) - 1, "localhost");
		}
		GetNamedPipeClientProcessId(pThArg->hNamedPipe, &clientProcessId);
		printf("Connect to client process id: %d on computer: %s\n", clientProcessId, clientName);
		//Read and Execute Loop
		while (!shutDown && ReadFile(hNamedPipe, &request, RQ_SIZE, &nXfer, NULL)) {
			printf("Command from client thread: %d. %s\n", clientProcessId, request.record);
			shutDown = shutDown || (strcmp((char*)request.record, shutRequest) == 0);
			if (shutDown)  continue;
			//Create Temp File
			hTmpFile = CreateFileA(pThArg->tempFileName, GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE, &tempSA,
				CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
			if (hTmpFile == INVALID_HANDLE_VALUE) { 
				printf("Cannot create temp file\n");
				_endthreadex(1);
			}

			//Create a process  Redirect IO
			startInfoCh.hStdOutput = hTmpFile;
			startInfoCh.hStdError = hTmpFile;
			startInfoCh.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
			startInfoCh.dwFlags = STARTF_USESTDHANDLES;

			if (!CreateProcessA(NULL, (char*)request.record, NULL,
				NULL, TRUE,
				0, NULL, NULL, &startInfoCh, &procInfo)) {
				printf("ERR: Cannot create process.\n");
				procInfo.hProcess = NULL;
			}

			CloseHandle(hTmpFile); 
			if (procInfo.hProcess != NULL) { 
				CloseHandle(procInfo.hThread);
				WaitForSingleObject(procInfo.hProcess, INFINITE);
				CloseHandle(procInfo.hProcess);
			}

		

			if (fopen_s(&fp, pThArg->tempFileName, "r") != 0) {
				printf("Temp output file is: %s.\n", pThArg->tempFileName);
				perror("Failure to open command output file./n");
				break;  
			}

			while (fgets((char*)response.record, MAX_RQRS_LEN, fp) != NULL) {
				response.rsLen = (LONG32)(strlen((char*)response.record) + 1);
				WriteFile(hNamedPipe, &response, response.rsLen + sizeof(response.rsLen), &nXfer, NULL);
			}
			//Write a terminating record.
			response.record[0] = '\0';
			response.rsLen = 0;
			WriteFile(hNamedPipe, &response, sizeof(response.rsLen), &nXfer, NULL);

			FlushFileBuffers(hNamedPipe);
			fclose(fp);

		} // End of main command loop.


		FlushFileBuffers(hNamedPipe);
		DisconnectNamedPipe(hNamedPipe);
	}

	//  Force terminate the connection thread 
	if (hConTh != NULL) {
		GetExitCodeThread(hConTh, &conThStatus);
		if (conThStatus == STILL_ACTIVE) {
			hClient = CreateFileA(SERVER_PIPE, GENERIC_READ | GENERIC_WRITE, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hClient != INVALID_HANDLE_VALUE) CloseHandle(hClient);
			WaitForSingleObject(hConTh, INFINITE);
		}
	}
	printf("Thread %d shutting down.\n", pThArg->threadNumber);
	//Clear up
	CloseHandle(hTmpFile); hTmpFile = INVALID_HANDLE_VALUE;
	if (!DeleteFileA(pThArg->tempFileName)) {
		printf("Failed deleting temp file.\n");
	}
	printf("Exiting server thread number %d.\n", pThArg->threadNumber);
	return 0;
}

DWORD WINAPI Connect(LPTHREAD_ARG pThArg)
{
	BOOL fConnect;
	fConnect = ConnectNamedPipe(pThArg->hNamedPipe, NULL);
	_endthreadex(0);
	return 0;
}

DWORD WINAPI ServerBroadcast(LPLONG pNull)
{
	MS_MESSAGE MsNotify;
	DWORD nXfer, iNp;
	HANDLE hMsFile;

	while (!shutDown) { 
		Sleep(CS_TIMEOUT);
		hMsFile = CreateFileA(MS_CLTNAME, GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hMsFile == INVALID_HANDLE_VALUE) continue;
		MsNotify.msStatus = 0;
		MsNotify.msUtilization = 0;
		strncpy_s(MsNotify.msName, sizeof(MsNotify.msName) / sizeof(CHAR), SERVER_PIPE, _TRUNCATE);
		if (!WriteFile(hMsFile, &MsNotify, MSM_SIZE, &nXfer, NULL))
		{
			printf("Server MS Write error.\n"); 
			CloseHandle(hMsFile); _endthreadex(0);
		}
		CloseHandle(hMsFile);
	}


	printf("Shut down flag set. Cancel all outstanding I/O operations.\n");
	for (iNp = 0; iNp < MAX_CLIENTS; iNp++) {
		CancelIoEx(threadArgs[iNp].hNamedPipe, NULL);
	}
	printf("Shuting down monitor thread.\n");

	_endthreadex(0);
	return 0;
}

BOOL WINAPI Handler(DWORD CtrlEvent)
{
	printf("In console control handler\n");
	InterlockedIncrement(&shutDown);
	return TRUE;
}
