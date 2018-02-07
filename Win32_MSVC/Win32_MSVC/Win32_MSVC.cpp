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
static bool exitFlag;

int main(int argc, LPCSTR argv[])
{
	if (argc == 1) {
		printf("Please Input Time Limit\n"); return 0;
	}
	HANDLE hJobObject;
	LARGE_INTEGER processTimeLimit;
	//processTimeLimit.QuadPart = 60;
	BOOL exitFlag = FALSE;
	CHAR command[MAX_COMMAND_LINE], *pc;
	DWORD i, localArgc; 
	CHAR argstr[MAX_ARG][MAX_COMMAND_LINE];
	LPSTR pArgs[MAX_ARG];
	for (i = 0; i < MAX_ARG; i++)
		pArgs[i] = argstr[i];
	printf("Simple Job Mangement\n");
	//job objects
	
	hJobObject = NULL;
	processTimeLimit.QuadPart = 0;
	if (argc >= 2) processTimeLimit.QuadPart = atoi((char*)argv[1]);
	basicLimits.PerProcessUserTimeLimit.QuadPart = processTimeLimit.QuadPart * MILLION;

	hJobObject = CreateJobObject(NULL, NULL);
	if (NULL == hJobObject)
	{
		printf("Error creating job object.\n"); return 0;
	}
	if (!SetInformationJobObject(hJobObject, JobObjectBasicLimitInformation, &basicLimits, sizeof(JOBOBJECT_BASIC_LIMIT_INFORMATION)))
	{
		printf("Error setting job object information.\n"); return 0;
	}
	//
	while (!exitFlag) {
		printf("%s", "JM$");
		fgets(command, MAX_COMMAND_LINE, stdin);
		pc = strchr(command, '\n');
		*pc = '\0';
		GetArgs(command, &localArgc, pArgs);
		CharLowerA(argstr[0]);

		if (strcmp(argstr[0], "jobbg") == 0) {
			Jobbg(localArgc, pArgs, command,hJobObject);
		}
		else if (strcmp(argstr[0], "jobs") == 0) {
			Jobs(localArgc, pArgs, command, hJobObject);
		}
		else if (strcmp(argstr[0], "kill") == 0) {
			Kill(localArgc, pArgs, command);
		}
		else if (strcmp(argstr[0], "quit") == 0) {
			exitFlag = TRUE;
		}
		else printf("Invalid command. Try again.\n");
	}
	CloseHandle(hJobObject);
	return 0;
}


