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
	BOOL exitFlag = FALSE;
	CHAR command[MAX_COMMAND_LINE], *pc;
	DWORD i, localArgc; 
	CHAR argstr[MAX_ARG][MAX_COMMAND_LINE];
	LPSTR pArgs[MAX_ARG];
	for (i = 0; i < MAX_ARG; i++)
		pArgs[i] = argstr[i];
	printf("Simple Job Mangement\n");
	while (!exitFlag) {
		printf("%s", "JM$");
		fgets(command, MAX_COMMAND_LINE, stdin);
		pc = strchr(command, '\n');
		*pc = '\0';
		GetArgs(command, &localArgc, pArgs);
		CharLowerA(argstr[0]);

		if (strcmp(argstr[0], "jobbg") == 0) {
			Jobbg(localArgc, pArgs, command);
		}
		else if (strcmp(argstr[0], "jobs") == 0) {
			Jobs(localArgc, pArgs, command);
		}
		else if (strcmp(argstr[0], "kill") == 0) {
			Kill(localArgc, pArgs, command);
		}
		else if (strcmp(argstr[0], "quit") == 0) {
			exitFlag = TRUE;
		}
		else printf("Invalid command. Try again.\n");
	}

	return 0;
}


