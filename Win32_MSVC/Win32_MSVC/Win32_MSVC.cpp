// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "Everything.h"
#include "ClientServer.h"	
#pragma comment(lib, "Ws2_32.lib")
static BOOL SendRequestMessage(REQUEST *, SOCKET);
static BOOL ReceiveResponseMessage(RESPONSE *, SOCKET);
struct sockaddr_in clientSAddr;
int main(int argc, LPCSTR argv[])
{
	SOCKET clientSock = INVALID_SOCKET;
	REQUEST request;	
	RESPONSE response;	
	WSADATA WSStartData;				
	BOOL quit = FALSE;
	DWORD conVal;

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

	if (argc < 2) {
		printf("Port is %d\n", SERVER_PORT); 
		clientSAddr.sin_port = htons(SERVER_PORT);
	}
	else {
		int port = atoi(argv[1]);
		printf("Port is %d\n", port);
		clientSAddr.sin_port = htons(port);
	}
	printf("Waiting connecting\n"); 
	conVal = connect(clientSock, (struct sockaddr *)&clientSAddr, sizeof(clientSAddr));
	if (conVal == SOCKET_ERROR) {
		printf("Failed client connect() call\n"); return 0;
	} 

	/*  Main loop to prompt user, send request, receive response */
	while (!quit) {
		printf("Enter Command: \n");
		fgets((char*)request.record, MAX_RQRS_LEN - 1, stdin);
		/* Get rid of the new line at the end */
		/* Messages use 8-bit characters */
		request.record[strlen((char*)request.record) - 1] = '\0';
		if (strcmp((char*)request.record, "$Quit") == 0) quit = TRUE;
		printf("Sending Request Message\n");
		SendRequestMessage(&request, clientSock);
		printf("Waiting ReceiveResponseMessage\n");
		if (!quit) ReceiveResponseMessage(&response, clientSock);
	}

	//clear up
	shutdown(clientSock, SD_BOTH); 

	closesocket(clientSock);
	WSACleanup();

	printf("****Leaving client\n");
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
