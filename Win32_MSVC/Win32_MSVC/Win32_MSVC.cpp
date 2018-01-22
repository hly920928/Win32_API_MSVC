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
	//char inFile[666] = "data.txt";
	//char outFile[666] = "output.txt";
	sortFileMapped(argv[1], argv[2]);
	return 0;
}


