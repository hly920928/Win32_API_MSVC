// Win32_MSVC.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdio>
#include "Everything.h" 
#include "myHeader_V2.h" 
#include "test.h" 
int main()
{
	__try {
		printf("In try\n");
	}
	__finally {
		printf("In finally\n");
	}
    return 0;
}

