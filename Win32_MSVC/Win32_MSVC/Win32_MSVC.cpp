// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "Everything.h" 
#include "myHeader_V2.h" 
#include <string>
#include "jobsMngmt.h";
#include "helperFunction.h"
using namespace std;
struct Record {
	char t[10];
	char operator[](int i)const {
		return t[i];
	}
	char& operator[](int i) {
		return t[i];
	}
	void printOut()const {
		for (int i = 0; i < 8; i++) {
			printf("%c", t[i]);
		}
		printf("\n");
	}
	bool operator <(const Record&  b) {
		for (int i = 0; i < 8; i++) {
			if (t[i] < b.t[i])return true;
			if (t[i] > b.t[i])return false;
		}
		return false;
	}
};
struct sort_thread_arg {
	unsigned char id;
	unsigned char threadNum;
	Record* head;
	Record* end;
};
void RandomRecord(Record* header, int n) {
	for (int i = 0; i < n; i++) {
		auto& temp= *header;
		for (int t = 0; t < 8; t++) {
			int tt = rand() % 10;
			temp[t] = tt+'0';
		}
		temp[8] = '\0';
		temp[9] = '\r\n';
		header++;
	}
}
static DWORD WINAPI sort_MT(sort_thread_arg* pArgs);
int main(int argc, LPCSTR argv[])
{
	if (argc < 3) {
		printf("Command Line incorrent\n");
		return 0;
	}
	char fileName[999];
	strcpy(fileName, argv[1]);
	int threadNum = atoi(argv[2]);
	
	//check threadNum
	{
		int threadNumber[] = {
			1,2,4,8,16,32,64,128,256 };
		bool f1 = false;
		for (int i : threadNumber) {
			if (threadNum == i) {
				f1 = true;
				break;
			}
		}
		if (!f1) {
			printf("threadNum InValid\n");
			goto END;
		}
	}
	//open file
	HANDLE hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE,0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Failure to open input file.\m");
		goto END;
	}
	long long fileSize = -1;
	GetFileSizeEx(hFile, (PLARGE_INTEGER)&fileSize);
	int nOfRecord = (fileSize) / sizeof(Record);
	HANDLE mHandle = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (NULL == mHandle) {
        printf("Failure to create mapping handle on input file.\n");
		goto END;
	}
	Record* pRecords = (Record*)MapViewOfFile(mHandle, FILE_MAP_ALL_ACCESS, 0, 0,0);
	RandomRecord(pRecords, nOfRecord);
	if (NULL == pRecords) {
		printf("Failure to map input file.\n");
		goto END;
	}
	Record* h = pRecords;
//Clear up	
END:
	UnmapViewOfFile(pRecords);
	CloseHandle(mHandle);
	CloseHandle(hFile);
	return 0;
}

static DWORD WINAPI sort_MT(sort_thread_arg* pArgs)
{
    
	return 0;
}
