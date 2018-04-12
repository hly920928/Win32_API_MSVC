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

int main(int argc, LPCSTR argv[])
{
	
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
