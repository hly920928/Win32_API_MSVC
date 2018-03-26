// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <ctime>
#include "SynchObj.h"
#include <iostream>
#include <string>
#include "messages.h"
#include  "ClientServer.h"
using namespace std;

int main(int argc, LPCSTR argv[])
{
	HANDLE hNamedPipe = INVALID_HANDLE_VALUE;
	CHAR quitMsg[] = "$Quit";
	CHAR serverPipeName[MAX_PATH + 1];
	REQUEST request;		
	RESPONSE response;	
	DWORD nRead, nWrite, npMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
	string str;
	/*
	while (true) {
		//processing Input
		printf("Enter Command: ");
		cin >> str;
		string t;
		while (true) {
			cin >> t;
			if (t == "END")break;
			str = str +" " +t;
		}
		if (!strcmp(str.data(), quitMsg))return 0;
		copy(str.data(), str.data() + str.size(), request.record);
		request.record[str.size()] = '\0';
		//printf("%s\n", (char*)request.record);
	}
	*/
	LocateServer(serverPipeName, MAX_PATH);

	while (INVALID_HANDLE_VALUE == hNamedPipe) { 
		if (!WaitNamedPipeA(serverPipeName, NMPWAIT_WAIT_FOREVER))
		{
			printf("WaitNamedPipe error.\n");	return 0;
		}
		hNamedPipe = CreateFileA(serverPipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if (!SetNamedPipeHandleState(hNamedPipe, &npMode, NULL, NULL))
	{
		printf("SetNamedPipeHandleState error.\n");	return 0;
	}
	request.rqLen = RQ_SIZE;

	while (true){
		//processing Input
		printf("Enter Command: ");
		cin >> str;
		string t;
		while (true) {
			cin >> t;
			if (t == "END")break;
			str = str + " " + t;
		}
		if (!strcmp(str.data(), quitMsg))return 0;
		copy(str.data(), str.data() + str.size(), request.record);
		request.record[str.size()] = '\0';
		//printf("%s\n", (char*)request.record);
		if (!WriteFile(hNamedPipe, &request, RQ_SIZE, &nWrite, NULL)) {
			printf("Write NP failed.\n");	return 0;
		}
	
		while (ReadFile(hNamedPipe, &response, RS_SIZE, &nRead, NULL))
		{
			if (response.rsLen <= 1) {break; }
			printf("%s\n", response.record);
		}
	}
	return 0;
};

