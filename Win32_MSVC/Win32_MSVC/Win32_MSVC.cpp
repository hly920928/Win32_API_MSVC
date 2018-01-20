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
	//Not defensive progromming
	HANDLE hIn = INVALID_HANDLE_VALUE, hNode = NULL, hData = NULL;
	LPTNODE pRoot;
	BOOL noPrint;
	CHAR errorMessage[256];
	char filename[666] = "data.txt";
	hIn = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
		printf("Open file fail\n"); return 0;
	}
	hNode = HeapCreate(
		HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE, NODE_HEAP_ISIZE, 0);
	hData = HeapCreate(
		HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE, DATA_HEAP_ISIZE, 0);
	pRoot = FillTree(hIn, hNode, hData);
	Scan(pRoot);
	return 0;
}


