// Win32_MSVC.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdio>
#include <iostream>
#include "Everything.h" 
#include "myHeader_V2.h" 
#include "float.h"
int main(int argc, LPCSTR argv[])
{	
	HANDLE hIn = INVALID_HANDLE_VALUE, hOut = INVALID_HANDLE_VALUE;
	HANDLE hInMap = NULL, hOutMap = NULL;
	char * pInFile = NULL;
	basedIndexData* pOutFile = NULL;
	LARGE_INTEGER fileSize;
	//char fileName[99] = "data.txt";
	//char indexName[99] = "index.txt";
	//bool isCreateNew = false;
	bool isCreateNew = argc > 3 && argv[3][0] == 'n';
	const char* fileName = argv[1];
	const char* indexName = argv[2];
	hIn = CreateFileA(fileName, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE)
	{
		printf("Open Infile Fail\n"); return FALSE;
	}
	if (!GetFileSizeEx(hIn, &fileSize)) {
		printf("GetFileSize() Fail\n");
		return FALSE;
	}
	hInMap = CreateFileMapping(hIn, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hInMap == NULL) {
		printf("hInMap CreateMap Fail\n"); return FALSE;
	}
	pInFile = (char*)MapViewOfFile(hInMap, FILE_MAP_READ, 0, 0, 0);
	//set Base pointer
	pBase = pInFile;
	if (pInFile == NULL) {
		printf("pInFile Map Fail\n");
		return FALSE;
	}
	if (isCreateNew) {
		hOut = CreateFileA(indexName, GENERIC_READ | GENERIC_WRITE,
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		printf("Creating New Index File\n");
	}
	else {
		hOut = CreateFileA(indexName, GENERIC_READ | GENERIC_WRITE,
			0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		printf("Open Existing Index File\n");
	}
	if (hOut == INVALID_HANDLE_VALUE) {
		printf("Open Outfile Fail\n"); return FALSE;
	}
	hOutMap = CreateFileMapping(hOut, NULL, PAGE_READWRITE, fileSize.HighPart, fileSize.LowPart, NULL);
	if (hOutMap == NULL) {
		printf("hInMap CreateMap Fail\n"); return FALSE;
	}
	pOutFile = (basedIndexData*)MapViewOfFile(hOutMap, FILE_MAP_WRITE, 0, 0, (SIZE_T)fileSize.QuadPart);
	if (pOutFile == NULL) {
		printf("pOutFile Map Fail\n"); return FALSE;
	}
	if (isCreateNew) {
		CreateIndexFile(fileSize, pOutFile, pInFile);
	}
	while (!isNull(pOutFile->key)) {
		pOutFile->printOut(pInFile);
		pOutFile++;
	}
	//Release And Close all
	UnmapViewOfFile(pOutFile); UnmapViewOfFile(pInFile);
	CloseHandle(hOutMap); CloseHandle(hInMap);
	CloseHandle(hIn); CloseHandle(hOut);
	return 0;
}


