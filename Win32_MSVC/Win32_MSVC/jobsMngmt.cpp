#include "stdafx.h"
#include "jobsMngmt.h";

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
		else printf("      ");
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
	CloseHandle(hJobData);
	return TRUE;
}

DWORD FindProcessId(DWORD jobNumber)
{
	return 0;
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
