// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "Everything.h"
#include "ClientServer.h"	
#include  <ctime>
#pragma comment(lib, "Ws2_32.lib")
#define LOCALHOST "127.0.0.1"
#define myPost 48500
struct data_8 {
	char str[8];
};
static BOOL  WINAPI Handler (DWORD);
bool sendDate(char* buffer, int len, SOCKET sk);
bool receiveDate(char* buffer, int len, SOCKET sk);
struct sockaddr_in clientSAddr;
struct sockaddr_in srvSAddr;
struct sockaddr_in connectSAddr;
WSADATA WSStartData;
volatile static unsigned int shutFlag = 0;
static SOCKET SrvSock = INVALID_SOCKET, connectSock = INVALID_SOCKET;
int main(int argc, LPCSTR argv[])
{
	SOCKET clientSock = INVALID_SOCKET;
	data_8 d1;
	WSADATA WSStartData;
	int conVal = 0;
	if (!SetConsoleCtrlHandler(Handler, TRUE)) {
		printf("Cannot create Ctrl handler\n"); return 0;
	}
	if (WSAStartup(MAKEWORD(2, 0), &WSStartData) != 0) {
		printf("Cannot support sockets\n"); return 0;
	}
	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET) {
		printf("Failed client socket() call\n"); return 0;
	}
	memset(&clientSAddr, 0, sizeof(clientSAddr));
	clientSAddr.sin_family = AF_INET;
	clientSAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (argc >= 2) {
		int port = atoi(argv[1]);
		clientSAddr.sin_port = htons(port);
	}
	else {
		clientSAddr.sin_port = htons(myPost);
	}
	conVal = connect(clientSock, (struct sockaddr *)&clientSAddr, sizeof(clientSAddr));
	if (conVal == SOCKET_ERROR) {
		printf("Failed client connect() call Port is %d\n", (int)ntohs(clientSAddr.sin_port)); return 0;
	}
	else {
		printf("client connect() succ\n");
	}
	srand(time(NULL));
	//sending and Receive Loop
	data_8 dt;
	while (!shutFlag) {
		Sleep(2000);
		int t = (rand());
		itoa(t, dt.str, 8);
		printf("Waiting Send %s\n", dt.str);
		if (sendDate(dt.str, 8, clientSock)) {
			printf("Send succ\n", dt.str);
		}
		else {
			printf("Send fail\n", dt.str);
			break;
		}
		printf("Waiting receive\n", dt.str);
		if (receiveDate(dt.str, 8, clientSock)) {
			printf("Receive succ %s\n", dt.str);
		}
		else {
			printf("Receive fail\n", dt.str);
			break;
		}
	}
	//clear up
	shutdown(clientSock, SD_BOTH);
	closesocket(clientSock);
	WSACleanup();
	printf("Leaving client\n");
	return 0;
	};
BOOL WINAPI Handler(DWORD CtrlEvent)
{
	printf("In console control handler\n");
	InterlockedIncrement(&shutFlag);
	return TRUE;
}

bool sendDate(char * buffer, int len, SOCKET sk)
{
	bool disconnect = false;
	int nRemain = len; int nTransfered = -1;
	char* pToCur= buffer;
	while (nRemain> 0 && !disconnect) {
		nTransfered = send(sk, pToCur, nRemain, 0);
		if (nTransfered == SOCKET_ERROR) {
			printf("send() failed\n"); return false;
		} 
		nRemain -= nTransfered; pToCur += nTransfered;
		disconnect = (nTransfered == 0) || (nRemain == 0);
	}
	return disconnect;
}

bool receiveDate(char * buffer, int len, SOCKET sk)
{
	bool disconnect = false;
	int nRemain = len; int nTransfered = -1;
	char* pToCur = buffer;
	while (nRemain> 0 && !disconnect) {
		nTransfered = recv(sk, pToCur, nRemain, 0);
		if (nTransfered == SOCKET_ERROR) {
			printf("receive() failed\n"); return false;
		}
		nRemain -= nTransfered; pToCur += nTransfered;
		disconnect = (nTransfered == 0)||(nRemain==0);
		
	}
	return disconnect;
}
