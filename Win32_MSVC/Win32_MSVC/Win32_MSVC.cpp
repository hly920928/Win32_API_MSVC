// Win32_MSVC.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdio>
#include "Everything.h" 
#include "myHeader_V2.h" 
int main(int argc, LPCSTR argv[])
{
	HANDLE hIn = INVALID_HANDLE_VALUE, hOut = INVALID_HANDLE_VALUE;
	DWORD nXfer, iFile, j;
	CHAR outFileName[256] = "", *pBuffer = NULL;
	OVERLAPPED ov = { 0, 0, 0, 0, NULL };
	LARGE_INTEGER fSize;
	if (argc <= 1) {
		printf("Usage: to_upper files Please Input file name\n");
		return 0;
	}
	for (iFile = 1; iFile < (unsigned int)argc; iFile++)
		try {
		if (strlen(argv[iFile]) > 250) {
			printf("The file name is too long./n");
			throw 1;
		}
		sprintf(outFileName, "UC_%s", argv[iFile]);
		try {
				hIn = CreateFileA(argv[iFile], GENERIC_READ,
					0, NULL, OPEN_EXISTING, 0, NULL);
				if (hIn == INVALID_HANDLE_VALUE) {
					printf("%d Invalid\n", argv[iFile]);
					throw 2;
				}
				if (!GetFileSizeEx(hIn, &fSize) || fSize.HighPart > 0) {
						printf("%s is too large for this program\n", argv[iFile]);
						throw 3;
				}
				hOut = CreateFileA(outFileName, GENERIC_READ | GENERIC_WRITE,
					0, NULL, CREATE_NEW, 0, NULL);
				if (hOut == INVALID_HANDLE_VALUE) {
					printf("%s open fail\n", argv[iFile]);
					throw 4;
				}
				pBuffer =(CHAR*) malloc(fSize.LowPart);
				if (pBuffer == NULL) {
					printf("%s malloc fail\n", argv[iFile]);
					throw 5;
				}
				if (!ReadFile(hIn, pBuffer, fSize.LowPart, &nXfer, NULL) || (nXfer != fSize.LowPart))
				{
					printf("%s Read fail\n", argv[iFile]);
					throw 6;
				}
				for (j = 0; j < fSize.LowPart; j++)
					if (isalpha(pBuffer[j])) pBuffer[j] = toupper(pBuffer[j]);
				if (!WriteFile(hOut, pBuffer, fSize.LowPart, &nXfer, NULL) || (nXfer != fSize.LowPart))
				{
					printf("%s Write fail\n", argv[iFile]);
					throw 6;
				}

			}
			catch(...){}
			if (pBuffer != NULL) free(pBuffer); pBuffer = NULL;
			if (hIn != INVALID_HANDLE_VALUE) {
				CloseHandle(hIn);
				hIn = INVALID_HANDLE_VALUE;
			}
			if (hOut != INVALID_HANDLE_VALUE) {
				CloseHandle(hOut);
				hOut = INVALID_HANDLE_VALUE;
			}
			strcpy(outFileName, "");
		}
	catch (...) {
		printf("Error occured processing file %s\n", argv[iFile]);
		DeleteFileA(outFileName);
	}
	printf("All files converted\n");
    return 0;
};

