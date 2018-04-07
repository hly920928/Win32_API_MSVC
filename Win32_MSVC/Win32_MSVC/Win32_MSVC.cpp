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

BOOL SendRequestMessage(REQUEST *, SOCKET)
{
	return 0;
}

BOOL ReceiveResponseMessage(RESPONSE *pResponse, SOCKET sd)
{
	BOOL disconnect = FALSE, LastRecord = FALSE;
	LONG32 nRemainRecv, nXfer;
	char* pBuffer;

	while (!LastRecord) {
		// Read the header 
		nRemainRecv = RS_HEADER_LEN; pBuffer =(char*)pResponse;
		while (nRemainRecv > 0 && !disconnect) {
			nXfer = recv(sd, pBuffer, nRemainRecv, 0);
			if (nXfer == SOCKET_ERROR) {
				printf("client response recv() failed\n"); return 0;
			}
			disconnect = (nXfer == 0);
			nRemainRecv -= nXfer; pBuffer += nXfer;
		}
		//finish reading pResponse->rsLen
		//	Read the response record 
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
