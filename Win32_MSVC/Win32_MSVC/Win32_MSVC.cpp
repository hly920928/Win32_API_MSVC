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
struct fiberArg {
	LPVOID*  fibs;
	int id;
};
static int num =7;
static DWORD WINAPI fiber_Test(fiberArg* pArgs);
int main(int argc, LPCSTR argv[])
{
	

	LPVOID FiberID[99];
	fiberArg fbArg[99];
	FiberID[0]=ConvertThreadToFiber(NULL);
	for (int i = 1; i <num; i++) {
		fbArg[i].fibs = FiberID;
		fbArg[i].id = i;
		FiberID[i]=CreateFiber(1, (LPFIBER_START_ROUTINE)fiber_Test, &fbArg[i]);
	}
	SwitchToFiber(FiberID[1]);
	printf("Back From Switch fiber 0\n");
	int t = 5;
	return 0;
}

DWORD WINAPI fiber_Test(fiberArg* pArgs)
{
	while (true) {
		printf("In fiber %d\n", pArgs->id);
		int nextID = rand() % num;
		SwitchToFiber((LPVOID)pArgs->fibs[nextID]);
		printf("Back From Switch fiber %d\n", pArgs->id);
	}
	return 0;
}
