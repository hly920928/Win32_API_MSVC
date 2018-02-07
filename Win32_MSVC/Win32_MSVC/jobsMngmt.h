#pragma once
#include "Everything.h" 
#define JM_EXIT_CODE 0x1000
#define MAX_JOBS_ALLOWED 10000
#define SJM_JOB sizeof(JM_JOB)

static JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimits = { 0, 0, JOB_OBJECT_LIMIT_PROCESS_TIME };
struct JM_JOB
{
	DWORD ProcessId;
	CHAR CommandLine[MAX_PATH];
};
LONG GetJobNumber(PROCESS_INFORMATION *pProcessInfo, LPCSTR Command);
BOOL DisplayJobs(void);
DWORD FindProcessId(DWORD jobNumber);
BOOL GetJobMgtFileName(LPSTR jobMgtFileName);

 int Jobbg(int, LPSTR *, LPSTR, HANDLE hJobObject);
 int Jobs(int, LPSTR *, LPSTR, HANDLE hJobObject);
 int Kill(int, LPSTR *, LPSTR);
