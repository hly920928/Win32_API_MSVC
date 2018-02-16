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
#define MAXTHREAD 256
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
	HANDLE* hArr;
	Record* head;
	Record* end;
	sort_thread_arg* argArr;
};
void RandomRecord(Record* header, int n) {
	Record* ptr = header;
	for (int i = 0; i < n; i++) {
		auto& temp= *ptr;
		for (int t = 0; t < 8; t++) {
			int tt = rand() % 10;
			temp[t] = tt+'0';
		}
		temp[8] = '\0';
		temp[9] = '\r\n';
		ptr++;
	}
	char t[16]= "AAAAAAAA\0\r\n";
	copy(t, t + 10, ptr->t);
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
	HANDLE hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE,
		                                            0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Failure to open input file.\n");
		goto END;
	}
	long long fileSize = -1;
	int nOfRecord = 60;
	if (!SetFilePointer(hFile, (nOfRecord + 1) * sizeof(Record), 0, FILE_BEGIN)|| !SetEndOfFile(hFile)) {
		printf("Failure to SetEndOfFile.\n");
		goto END;
	}
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
	HANDLE aHandle[MAXTHREAD];
	sort_thread_arg argArr[MAXTHREAD];
	int part = nOfRecord / threadNum;
	for (int i = 0; i < threadNum; i++) {
		argArr[i].id = i;
		argArr[i].threadNum = threadNum;
		argArr[i].hArr = aHandle;
		argArr[i].argArr = argArr;
		argArr[i].head = pRecords + i*part;
		if (i != threadNum - 1) 
			argArr[i].end = pRecords + (i + 1)*part;
		else argArr[i].end = pRecords + nOfRecord;

		aHandle[i]= (HANDLE)_beginthreadex(
			NULL, 0,(_beginthreadex_proc_type) sort_MT, &argArr[i], 
			CREATE_SUSPENDED, NULL);
	}
	for (int i= 0; i < threadNum; i++)
		ResumeThread(aHandle[i]);
	//WaitForMultipleObjects(threadNum, aHandle, TRUE, INFINITE);
	WaitForSingleObject(aHandle[0], INFINITE);
	
//Clear up	
END:
	for (int i = 0; i < threadNum; i++)
		CloseHandle(aHandle[i]);
	UnmapViewOfFile(aHandle);
	CloseHandle(mHandle);
	CloseHandle(hFile);
	return 0;
}

static DWORD WINAPI sort_MT(sort_thread_arg* pArgs)
{
	sort(pArgs->head, pArgs->end);
	int d = 1; bool flag = false;
	while (true) {
		flag = false;
		for (int i = 0; i < pArgs->threadNum; i += d * 2) {
			if (d >= pArgs->threadNum)break;
			if (pArgs->id==i) {
				flag = true;
				break;
			}
		}
		if (!flag)break;
		int nextID = pArgs->id + d;
		WaitForSingleObject(pArgs->hArr[nextID], INFINITE);
		int totalNum = pArgs->argArr[nextID].end - pArgs->head;
		Record* mergePlace = new Record[totalNum];
		merge(pArgs->head, pArgs->end, 
			      pArgs->argArr[nextID].head, pArgs->argArr[nextID].end,
			      mergePlace);
		copy(mergePlace, mergePlace + totalNum, pArgs->head);
		pArgs->end = pArgs->argArr[nextID].end;
		delete[]mergePlace;
		d *= 2;
	}
	return 0;
}
