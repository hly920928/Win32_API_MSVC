// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "Everything.h"
#include "ClientServer.h"	
#pragma comment(lib, "Ws2_32.lib")
#define LOCALHOST "127.0.0.1"
#define myPost 48500
struct data_8 {
	char str[8];
};
bool sendDate(char* buffer, int len, SOCKET sk);
bool receiveDate(char* buffer, int len, SOCKET sk);
struct sockaddr_in clientSAddr;
int main(int argc, LPCSTR argv[])
{
	SOCKET clientSock = INVALID_SOCKET;
	data_8 d1;
	WSADATA WSStartData;
	int conVal = 0;
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
	//clear up
	shutdown(clientSock, SD_BOTH);
	closesocket(clientSock);
	WSACleanup();
	close(clientSock);
	printf("Leaving client\n");
	return 0;
};

bool sendDate(char * buffer, int len, SOCKET sk)
{
	bool disconnect = false;
	int nRemain = len; int nTransfered = -1;
	char* pToCur= buffer;
	while (nRemain> 0 && !disconnect) {
		nTransfered = send(sk, pToCur, nRemain, 0);
		if (nTransfered == SOCKET_ERROR) {
			printf("client send() failed\n"); return false;
		} 
		disconnect = (nTransfered == 0);
		nRemain -= nTransfered; pToCur += nTransfered;
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
			printf("client send() failed\n"); return false;
		}
		disconnect = (nTransfered == 0);
		nRemain -= nTransfered; pToCur += nTransfered;
	}
	return disconnect;
}
