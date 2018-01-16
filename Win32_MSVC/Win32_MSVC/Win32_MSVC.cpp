// Win32_MSVC.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdio>
#include "Everything.h" 
#include "myHeader_V2.h" 
#include "test.h" 
int main(int argc, LPTSTR argv[])
{
	printf("%s\n", argv[0]);
	printf("%d\n", argc);
	for (int i = 1; i < argc; i++) 
		printf("%s\n", argv[i]);
    return 0;
}

