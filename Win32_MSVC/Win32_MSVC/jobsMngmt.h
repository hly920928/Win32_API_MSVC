#pragma once
#include "Everything.h" 
#define JM_EXIT_CODE 0x1000
#define MAX_JOBS_ALLOWED 10000
#define SJM_JOB sizeof(JM_JOB)
struct JM_JOB
{
	DWORD ProcessId;
	CHAR CommandLine[MAX_PATH];
};
LONG GetJobNumber(PROCESS_INFORMATION *pProcessInfo, LPCSTR Command);
BOOL DisplayJobs(void);
DWORD FindProcessId(DWORD jobNumber);
BOOL GetJobMgtFileName(LPSTR jobMgtFileName);

static int Jobbg(int, LPSTR *, LPSTR);
static int Jobs(int, LPSTR *, LPSTR);
static int Kill(int, LPSTR *, LPSTR);
