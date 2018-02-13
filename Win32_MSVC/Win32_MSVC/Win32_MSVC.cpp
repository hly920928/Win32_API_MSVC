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
	string pattern;

}
typedef   ptr_thread_arg (thread_arg*) ;
static DWORD WINAPI patternSearch_MT(thread_arg* pArgs);
int main(int argc, LPCSTR argv[])
{

	vector<vector<string>>output;
	//output.resize(argc - 2);
	output.resize(1);
	thread_arg* pthread_arg = new thread_arg();
	pthread_arg->id = 0;
	pthread_arg->output = &output;
	pthread_arg->pattern = "123456";
	pthread_arg->file_n = "1.txt";
	HANDLE tHandle[1024];
	tHandle[0] =(HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)patternSearch_MT, pthread_arg, 0, NULL);
	WaitForSingleObject(tHandle[0], INFINITE);
	int a = 6;
	return 0;
}

static DWORD WINAPI patternSearch_MT(thread_arg* pArgs)
{
	int tid = GetCurrentThreadId();
	printf("tid = %d\n", tid);
	ifstream fi; fi.open(pArgs->file_n);
	if (fi.fail()) {
		printf("Open_File_Fail\n");
	}
	int id = pArgs->id;
	string s;
	string pt = pArgs->pattern; const char* p= nullptr;
	while (true) {
		fi >> s;
		if (fi.eof())break;
		p=strstr(s.data(), pt.data());
		if (p != nullptr) {
			auto& v1=*pArgs->output;
			auto& v2 = v1[id];
			v2.push_back(s);
		}
	}
		/*
		  FILE *fp;
		  char file[] = "1.txt";
		  if ((fp = fopen(file, "rb")) == NULL) {

		  }
		*/
		delete pArgs;
		fi.close();
	return 0;
}
