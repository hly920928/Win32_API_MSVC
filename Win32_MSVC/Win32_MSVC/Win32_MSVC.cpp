// Win32_MSVC.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include <ctime>
#include "SynchObj.h"
#include <iostream>
#include <string>
#include "messages.h"
using namespace std;
int main(int argc, LPCSTR argv[])
{
	HANDLE hRead = GetStdHandle(STD_INPUT_HANDLE);
	printf("In ReadP\n");
	char buffer[6];
	string s;
	DWORD n_read = 0;
	while (true) {
		ReadFile(hRead, buffer, 6, &n_read, NULL);
		buffer[5] = '\0';
		s= buffer;
		if (buffer[0] == '-')break;
		cout << "In ReadP : " << s << endl;
	}
	printf("In ReadP END\n");
	return 0;
}

