// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <ctime>
#include "SynchObj.h"
#include <iostream>
#include <string>
#include "messages.h"
#include  "ClientServer.h"
#define myMS "\\\\.\\MAILSLOT\\temp_test_MailSlot"
using namespace std;
static BOOL SendRequestMessage(REQUEST *, SOCKET);
static BOOL ReceiveResponseMessage(RESPONSE *, SOCKET);
int main(int argc, LPCSTR argv[])
{
	return 0;
};

BOOL SendRequestMessage(REQUEST * pRequest, SOCKET sd)
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

BOOL ReceiveResponseMessage(RESPONSE *pResponse, SOCKET sd)
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
