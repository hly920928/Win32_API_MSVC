// Win32_MSVC.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdio>
#include "Everything.h" 
#include "myHeader_V2.h" 
#include "test.h" 
int main(int argc, LPCSTR argv[])
{
	printf("%s\n", argv[0]);
	printf("%d\n", argc);
	for (int i = 1; i < argc; i++) 
		printf("%s\n", argv[i]);
	if (argv[1] == NULL)
	{
		printf("No file Name\n");
		return 0;
	}
	//char name[] = "123.txt\0";
	//printf(" file Name %s\n", name);
	HANDLE hIn = CreateFileA(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
		printf("Open file fail\n");
		return 0;
	}
    DWORD nln = 0;
	char aBuffer[99];
	ReadFile(hIn, aBuffer, BUF_SIZE, &nln, NULL);
	printf("nIn = %d\n", nln);
	aBuffer[nln] = '\0';
	printf_s("%s\n", aBuffer); 
	
	    
    return 0;
}

