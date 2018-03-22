// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <ctime>
#include "SynchObj.h"
#include "messages.h"
using namespace std;
int main(int argc, LPCSTR argv[])
{
	HANDLE hWrite = GetStdHandle(STD_OUTPUT_HANDLE);
	if (argc < 3){
		printf("Input Error\n");
		return 0;
	}
	printf("In RandInt\n");
	int num = atoi(argv[1]);
	int inval = atoi(argv[2]);
	srand((int)time(NULL));
	char buffer[6];
	for (int i = 0; i < num; i++) {
		Sleep(inval);
		int n = rand() % 10000;
		printf("In RandInt : %d\n", n);
		itoa(n, buffer,10);
		buffer[5] = '\n';
		WriteFile(hWrite, &buffer, sizeof(buffer), NULL, NULL);
	}
	char buffer_2[3];
	itoa(-1, buffer_2, 10);
	buffer[2] = '\n';
	WriteFile(hWrite, &buffer_2, sizeof(buffer_2), NULL, NULL);
	printf("In RandInt END\n");
	return 0;
}

