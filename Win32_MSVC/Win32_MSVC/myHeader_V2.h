#pragma once
#include "Everything.h"
#include <stdarg.h>
#define BUF_SIZE 0x200
#define STRING_SIZE 200
#define MAX_OPTION 6666
extern "C"

struct _RECORD {
	DWORD			referenceCount;
	SYSTEMTIME		recordCreationTime;
	SYSTEMTIME		recordLastRefernceTime;
	SYSTEMTIME		recordUpdateTime;
	CHAR			dataString[STRING_SIZE];
};
struct _HEADER { /* File header descriptor */
	DWORD			numRecords;
	DWORD			numNonEmptyRecords;
};
void ReportError(LPCWSTR userMessage, DWORD exitCode,
	BOOL printErrorMessage);
void CatFile(HANDLE, HANDLE);
BOOL PrintStrings(HANDLE hOut, ...);
BOOL PrintMsg(HANDLE hOut, const char* pMsg);
BOOL ConsolePrompt(const char* pPromptMsg, LPTSTR pResponse, DWORD maxChar, BOOL echo);
BOOL TraverseDirectory(LPCWSTR, LPCWSTR, DWORD, LPBOOL);
DWORD FileType(LPWIN32_FIND_DATA);
BOOL ProcessItem(LPWIN32_FIND_DATA, DWORD, LPBOOL);
//About Registry
BOOL TraverseRegistry(HKEY hKey, LPTSTR fullKeyName, LPTSTR subKey, LPBOOL flags);
BOOL DisplayPair(LPTSTR valueName, DWORD valueType, LPBYTE value, DWORD valueLen, LPBOOL flags);
BOOL DisplaySubKey(LPTSTR keyName, LPTSTR subKeyName, PFILETIME pLastWrite, LPBOOL flags);
void ReportException(LPCWSTR userMessage, DWORD exceptionCode);
DWORD ErrorFilter(LPEXCEPTION_POINTERS pExP, LPDWORD eCategory);
BOOL WINAPI CtrlHandler(DWORD cntrlEvent);