#include "stdafx.h"
#include "jobsMngmt.h";
#include "helperFunction.h";
#include "myHeader_V2.h" 
LONG GetJobNumber(PROCESS_INFORMATION * pProcessInfo, LPCSTR Command)
{
	HANDLE hJobData, hProcess;
	JM_JOB jobRecord;
	DWORD jobNumber = 0, nXfer, exitCode;
	LARGE_INTEGER fileSize, fileSizePlus;
	CHAR jobMgtFileName[MAX_PATH];
	OVERLAPPED regionStart;
	if (!GetJobMgtFileName(jobMgtFileName)) return -1;
	hJobData = CreateFileA(jobMgtFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hJobData == INVALID_HANDLE_VALUE) {
		printf("JobData Open fail\n");
		return -1;
	}
	regionStart.Offset = 0;
	regionStart.OffsetHigh = 0;
	regionStart.hEvent = (HANDLE)0;
	GetFileSizeEx(hJobData, &fileSize);
	fileSizePlus.QuadPart = fileSize.QuadPart + SJM_JOB;
	bool LockOK=LockFileEx(hJobData, LOCKFILE_EXCLUSIVE_LOCK,0, fileSizePlus.LowPart, fileSizePlus.HighPart, &regionStart);
	if (!LockOK) 	return -1;
	while (jobNumber < MAX_JOBS_ALLOWED &&
		ReadFile(hJobData, &jobRecord, SJM_JOB, &nXfer, NULL) && (nXfer > 0)) {
		if (jobRecord.ProcessId == 0) break;//empty slot
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, jobRecord.ProcessId);
		if (hProcess == NULL) break;//invalid slot
		if (GetExitCodeProcess(hProcess, &exitCode)
			&& CloseHandle(hProcess) && (exitCode != STILL_ACTIVE))//End slot
			break;
		jobNumber++;
	}
	if (jobNumber >= MAX_JOBS_ALLOWED) return -1;
	if (nXfer != 0)
		SetFilePointer(hJobData, -(LONG)SJM_JOB, NULL, FILE_CURRENT);
	jobRecord.ProcessId = pProcessInfo->dwProcessId;
	strcpy_s(jobRecord.CommandLine,sizeof(jobRecord.CommandLine), Command);
	WriteFile(hJobData, &jobRecord, SJM_JOB, &nXfer, NULL);
	UnlockFileEx(hJobData, 0, fileSizePlus.LowPart, fileSizePlus.HighPart, &regionStart);
	CloseHandle(hJobData);
	return jobNumber + 1;
}

BOOL DisplayJobs(void)
{
	HANDLE hJobData, hProcess;
	JM_JOB jobRecord;
	DWORD jobNumber = 0, nXfer, exitCode;
	CHAR jobMgtFileName[MAX_PATH];
	OVERLAPPED regionStart;
	if (!GetJobMgtFileName(jobMgtFileName))
		return FALSE;
	hJobData = CreateFileA(jobMgtFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hJobData == INVALID_HANDLE_VALUE)
		return FALSE;
	regionStart.Offset = 0;
	regionStart.OffsetHigh = 0;
	regionStart.hEvent = (HANDLE)0;
	LockFileEx(hJobData, LOCKFILE_EXCLUSIVE_LOCK, 0, 0, 0, &regionStart);
	while (ReadFile(hJobData, &jobRecord, SJM_JOB, &nXfer, NULL) && (nXfer > 0)) {
		jobNumber++;
		hProcess = NULL;
		if (jobRecord.ProcessId == 0) continue;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, jobRecord.ProcessId);
		if (hProcess != NULL) {
			GetExitCodeProcess(hProcess, &exitCode);
			CloseHandle(hProcess);
		}
		printf(" [%d] ", jobNumber);
		if (NULL == hProcess)printf(" Done");
		else if (exitCode != STILL_ACTIVE)
			printf("+ Done");
		else printf("     ");
		printf(" %s\n", jobRecord.CommandLine);
		if (NULL == hProcess) {
			SetFilePointer(hJobData, -(LONG)nXfer, NULL, FILE_CURRENT);
			jobRecord.ProcessId = 0;
			if (!WriteFile(hJobData, &jobRecord, SJM_JOB, &nXfer, NULL))
			{
				printf("Write Error\n"); return 0;
			}
		}
	}
	UnlockFileEx(hJobData, 0, 0, 0, &regionStart);
	if (NULL != hProcess) CloseHandle(hProcess);
	if (hJobData != INVALID_HANDLE_VALUE)
	    CloseHandle(hJobData);
	return TRUE;
}

DWORD FindProcessId(DWORD jobNumber)
{
	HANDLE hJobData;
	JM_JOB jobRecord;
	DWORD nXfer, fileSizeLow;
	CHAR jobMgtFileName[MAX_PATH + 1];
	OVERLAPPED regionStart;
	LARGE_INTEGER fileSize;
	if (!GetJobMgtFileName(jobMgtFileName)) return 0;
	hJobData = CreateFileA(jobMgtFileName, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hJobData == INVALID_HANDLE_VALUE) return 0;
	if (!GetFileSizeEx(hJobData, &fileSize) ||
		(fileSize.HighPart != 0 ||  //InVaild fileSize
			SJM_JOB * (jobNumber - 1) > fileSize.LowPart //InVaild jobNumber
			|| fileSize.LowPart > SJM_JOB * MAX_JOBS_ALLOWED))
		return 0;
	fileSizeLow = fileSize.LowPart;
	SetFilePointer(hJobData, SJM_JOB * (jobNumber - 1), NULL, FILE_BEGIN);
	regionStart.Offset = SJM_JOB * (jobNumber - 1);
	regionStart.OffsetHigh = 0; 
	regionStart.hEvent = (HANDLE)0;
	LockFileEx(hJobData, 0, 0, SJM_JOB, 0, &regionStart);
	if (!ReadFile(hJobData, &jobRecord, SJM_JOB, &nXfer, NULL))
	{
		printf("ReadFile Fail\n"); return 0;
	}
	UnlockFileEx(hJobData, 0, SJM_JOB, 0, &regionStart);
	CloseHandle(hJobData);
	return jobRecord.ProcessId;
}

BOOL GetJobMgtFileName(LPSTR jobMgtFileName)
{
	CHAR UserName[MAX_PATH], TempPath[MAX_PATH];
	DWORD UNSize = MAX_PATH, TPSize = MAX_PATH;
	if (!GetUserNameA(UserName, &UNSize))
		return FALSE;
	if (GetTempPathA(TPSize, TempPath) > TPSize)
		return FALSE;
	sprintf_s(jobMgtFileName, MAX_PATH, "%s%s%s", TempPath, UserName, ".JobMgt");
	return TRUE;
}

int Jobbg(int argc, LPSTR* argv, LPSTR command)
{
	printf("argc=%d\n", argc);
	printf("argv= \n", argc);
	for(int i=0;i<argc;i++)
		printf("%s ", argv[i]);
	printf("\n", argc);
	printf("command = %s\n", command);
	//parse argv
	DWORD fCreate;
	LONG jobNumber;
	BOOL flags[2];
	STARTUPINFOA StartUp;
	PROCESS_INFORMATION processInfo;
	LPSTR targv = SkipArg(command, 1, argc, argv);
	GetStartupInfoA(&StartUp);
	Options(argc, argv, "cd", &flags[0], &flags[1], NULL);
	if (argv[1][0] == '-')
		targv = SkipArg(command, 2, argc, argv);
	fCreate = flags[0] ? CREATE_NEW_CONSOLE : flags[1] ? DETACHED_PROCESS : 0;
	//////////////
	if (!CreateProcessA(NULL, targv, NULL, NULL, TRUE,
		fCreate | CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP,
		NULL, NULL, &StartUp, &processInfo)) {
		printf("Create Process Fail\n");

		return 4;
	}
	//////////////
	jobNumber = GetJobNumber(&processInfo, targv);
	if (jobNumber >= 0)
		ResumeThread(processInfo.hThread);
	else {
		TerminateProcess(processInfo.hProcess, 3);
		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);
		printf("Error: No room in job control list.\n");
		return 5;
	}
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
	printf(" [%d] %d\n", jobNumber, processInfo.dwProcessId);
	return 0;
}

int Jobs(int, LPSTR *, LPSTR)
{
	if (!DisplayJobs()) return 1;
	return 0;
}


int Kill(int argc, LPSTR* argv, LPSTR command)
{
	printf("argc=%d\n", argc);
	printf("argv= \n", argc);
	for (int i = 0; i<argc; i++)
		printf("%s ", argv[i]);
	printf("\n", argc);
	printf("command = %s\n", command);
	///parse input
	DWORD processId, jobNumber, iJobNo;
	HANDLE hProcess;
	BOOL cntrlC, cntrlB, killed;
	iJobNo = Options(argc, argv, "bc", &cntrlB, &cntrlC, NULL);
	jobNumber = atoi(argv[iJobNo]);
	///
	processId = FindProcessId(jobNumber);
	if (processId == 0) {
		printf("Job number not found.\n");
		return 1;
	}
	hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
	if (hProcess == NULL) {
		printf("Process already terminated.\n");
		return 2;
	}
	killed = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, processId);
	/*
	if (cntrlB)
		killed = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0);
	else if (cntrlC)
		killed = GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
	else
		killed = TerminateProcess(hProcess, JM_EXIT_CODE);
	*/
	if (!killed) {
		printf("Process termination failed.\n");
		return 3;
	}
	WaitForSingleObject(hProcess, 5000);
	CloseHandle(hProcess);
	printf("Job [%d] terminated or timed out\n", jobNumber);
	return 0;
}
