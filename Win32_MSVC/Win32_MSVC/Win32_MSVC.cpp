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
using namespace std;
struct thread_arg {
	int id;
	vector<vector<string>>* output;
	string file_n;
	string* pattern;
};
static DWORD WINAPI patternSearch_MT(thread_arg* pArgs);
int main(int argc, LPCSTR argv[])
{
	//output.resize(argc - 2);
	if (argc < 3) {
		printf("Input_Error\n");
		return 0;
	}
	vector<vector<string>>output;
	output.resize(argc-2);
	string pattern = argv[1];
	HANDLE tHandle[1024];
	int threadCount = argc - 2;
	for (int i = 0; i < threadCount;i++) {
		thread_arg* p_thread_arg = new thread_arg();
		p_thread_arg->id = i;
		p_thread_arg->output = &output;
		p_thread_arg->pattern = &pattern;
		p_thread_arg->file_n = argv[2+i];
		tHandle[i] = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)patternSearch_MT, p_thread_arg, 0, NULL);
	}
	while (threadCount > 0) {
		int tID= WaitForMultipleObjects(threadCount, tHandle, FALSE, INFINITE);
		printf("Search %s Finish\n", argv[2 + tID]);
		printf("Result = \n", argv[2 + tID]);
		for (auto& s : output[tID]) {
			printf("%s\n", s.data());
		}
		printf("\n");
		threadCount--;
	}
	for (int i = 0; i < threadCount; i++) {
		CloseHandle(tHandle[i]);
	}
	int a = 6;
	return 0;
}

static DWORD WINAPI patternSearch_MT(thread_arg* pArgs)
{
	int tid = GetCurrentThreadId();
	//printf("tid = %d\n", tid);
	ifstream fi; fi.open(pArgs->file_n);
	if (fi.fail()) {
		printf("Open_File_Fail\n");
	}
	int id = pArgs->id;
	string s;
	string& pt =*(pArgs->pattern); const char* p= nullptr;
	auto& v1 = *pArgs->output;
	auto& v2 = v1[id];
	while (true) {
		fi >> s;
		if (fi.eof())break;
		p=strstr(s.data(), pt.data());
		if (p != nullptr) {
			v2.push_back(s);
		}
	}
		delete pArgs;
		fi.close();
	return 0;
}
