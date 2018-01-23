// Win32_MSVC.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdio>
#include <iostream>
#include "Everything.h" 
#include "myHeader_V2.h" 
typedef void(*ptr1)(char*) ;
int main(int argc, LPCSTR argv[])
{	
	ptr1 pPrintOut1;
	ptr1 pPrintOut2;
	HMODULE hDll = LoadLibraryA("DllTest.dll");
	if (hDll == NULL) {
		printf("LoadDll fail\n"); return 0;
	}
	pPrintOut1 = (ptr1)GetProcAddress(hDll,"printOut_V2");
	pPrintOut2 = (ptr1)GetProcAddress(hDll, "printOut_V1");
	if (pPrintOut1 == NULL|| pPrintOut2 == NULL) {
		printf("GetProcAddress fail\n"); return 0;
	}
	pPrintOut1((char*)argv[0]);
	pPrintOut2((char*)argv[0]);
	printf("End\n");
	FreeLibrary(hDll);
	return 0;
}


