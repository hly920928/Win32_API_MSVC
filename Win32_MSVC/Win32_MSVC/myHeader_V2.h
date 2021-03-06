#pragma once
#include "Everything.h"
#include <stdarg.h>
#include <algorithm>
#define BUF_SIZE 0x200
#define STRING_SIZE 200
#define MAX_OPTION 6666
extern "C"
#define KEY_SIZE 8
struct _TREENODE {
	struct _TREENODE *Left, *Right;
	CHAR key[KEY_SIZE];
	char* pData;
};
#define TREENODE _TREENODE
#define LPTNODE  _TREENODE*
#define LPPTNODE _TREENODE**
#define NODE_SIZE sizeof (TREENODE)
#define NODE_HEAP_ISIZE 0x8000
#define DATA_HEAP_ISIZE 0x8000
#define printf printf_s
#define MAX_DATA_LEN 0x1000
#define TKEY_SIZE KEY_SIZE * sizeof (CHAR)
#define ERROR_STATUS_FILE 0xE0000001
#define MILLION 1000000
static int nFile;
LPTNODE FillTree(HANDLE, HANDLE, HANDLE);
BOOL Scan(LPTNODE);
int KeyCompare(LPCSTR, LPCSTR);
BOOL InsertTree(LPPTNODE, LPTNODE);
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
struct word { /* File header descriptor */
	char s[9];
	bool operator<(const word& b) {
		for (int i = 0; i < 9; i++) {
			if (s[i] < b.s[i])return true;
			if (s[i] >b.s[i])return false;
		}
		return false;
	}
	void operator=(const word& b) {
		std::copy(b.s, b.s+ 9, s);
	}
	bool isNull() {
		for (int i = 0; i < 9; i++)
			if (s[i] != '\0')return false;
		return true;
	}
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
//About Memory File Mapping
BOOL cci_fileMapped(LPCSTR fIn, LPCSTR fOut, DWORD shift);
BOOL sortFileMapped(LPCSTR fIn, LPCSTR fOut);
static void* pBase;
struct basedIndexData {
	char key[8];
	long long offset;
	bool operator<(const basedIndexData&b) {
		for (int i = 0; i < 8; i++)
		{
			if (key[i] < b.key[i])return true;
			if (key[i] > b.key[i])return false;
		}
		return false;
	}
	void printOut(char* ptr)const {
		char *cur = ptr + offset;
		while (isalnum(*cur)) {
			printf("%c", *cur);
			cur++;
		}
		printf("\n");
	}
};
bool isNull(const char* p);
void CreateIndexFile(LARGE_INTEGER inputSize, basedIndexData* pIndexFile, LPSTR pInFile);

