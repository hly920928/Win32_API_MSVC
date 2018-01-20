#include "stdafx.h"
#include "myHeader_V2.h" 

void ReportError(LPCWSTR userMessage, DWORD exitCode,
	BOOL printErrorMessage) {
	DWORD eMegLen, errNum = GetLastError();
	LPTSTR lpvSysMsg;
	fprintf(stderr, "%s\n", userMessage);
	if (printErrorMessage) {
		eMegLen = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, errNum,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpvSysMsg, 0, NULL);
		if (eMegLen>0) {
			fprintf(stderr, "%s\n", lpvSysMsg);
		}
		else {
			fprintf(stderr, "Last Error Number;%d.\n", errNum);
		}
		if (lpvSysMsg != NULL)LocalFree(lpvSysMsg);
	}
	if (exitCode>0)ExitProcess(exitCode);
}

void CatFile(HANDLE hInFile, HANDLE hOutFile) {
	DWORD nIn, nOut;
	BYTE buffer[BUF_SIZE];
	while (ReadFile(hInFile, buffer, BUF_SIZE, &nIn, NULL)
		&& (nIn != 0)
		&& WriteFile(hOutFile, buffer, nIn, &nOut, NULL)
		);
	return;
}
BOOL cci_f(LPCWSTR fIn, LPCWSTR fOut, DWORD shift) {
	HANDLE hIn, hOut;
	DWORD nIn, nOut, iCopy;
	CHAR aBuffer[BUF_SIZE], ccBuffer[BUF_SIZE];
	BOOL writeOK = true;
	hIn = CreateFile(fIn, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE)return false;
	hOut = CreateFile(fOut, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOut == INVALID_HANDLE_VALUE)return false;
	while (ReadFile(hIn, aBuffer, BUF_SIZE, &nIn, NULL)
		&& nIn>0 && writeOK) {
		for (iCopy = 0; iCopy<nIn; iCopy++)
			ccBuffer[iCopy] = (aBuffer[iCopy] + shift) % 256;
		writeOK = WriteFile(hOut, ccBuffer, nIn, &nOut, NULL);
	}
	CloseHandle(hIn);  CloseHandle(hOut);
	return writeOK;
}
BOOL PrintStrings(HANDLE hOut, ...) {
	DWORD msgLen, count;
	const char* pMsg;
	va_list pMsgList;
	va_start(pMsgList, hOut);
	while ((pMsg = va_arg(pMsgList, const char*)) != NULL) {
		msgLen = strlen(pMsg);
		if (!WriteConsole(hOut, pMsg, msgLen, &count, NULL)) {
			if (!WriteFile(hOut, pMsg, msgLen * sizeof(char), &count, NULL)) {
				va_end(pMsgList); return FALSE;
			}
		}
	}
	va_end(pMsgList); return TRUE;
}
BOOL PrintMsg(HANDLE hOut, const char* pMsg) {
	return PrintStrings(hOut, pMsg, NULL);
}
BOOL ConsolePrompt(const char* pPromptMsg, LPTSTR pResponse, DWORD maxChar, BOOL echo) {
	HANDLE hIn, hOut;
	DWORD charIn, echoFlag;
	BOOL success;
	hIn = CreateFile(_T("CONIN$"),
		GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	hOut = CreateFile(_T("CONOUT$"),
		GENERIC_WRITE, 0,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	echoFlag = echo ? ENABLE_ECHO_INPUT : 0;
	success = SetConsoleMode(hIn,
		ENABLE_LINE_INPUT | echoFlag | ENABLE_PROCESSED_INPUT)
		&& SetConsoleMode(hOut,
			ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_PROCESSED_INPUT)
		&& PrintStrings(hOut, pPromptMsg, NULL)
		&& ReadConsole(hIn, pResponse, maxChar - 2, &charIn, NULL);
	if (success)
		pResponse[charIn - 2] = '\0';
	else ReportError(_T("ConsolePrompt failure."), 0, TRUE);
	CloseHandle(hIn);    CloseHandle(hOut);
	return success;
}
BOOL TraverseDirectory(LPCWSTR FileName, LPCWSTR DirName, DWORD numFlags, LPBOOL flags) {
	HANDLE searchHandle;
	WIN32_FIND_DATA findData;
	BOOL recursive = flags[0];
	DWORD fType, iPass;
	TCHAR currPath[MAX_PATH + 1];
	GetCurrentDirectory(MAX_PATH, currPath);
	for (iPass = 1; iPass <= 2; iPass++) {
		searchHandle = (iPass == 1) ? FindFirstFile(FileName, &findData) :
			FindFirstFile(DirName, &findData);
		do {
			fType = FileType(&findData);
			if (iPass == 1)
				ProcessItem(&findData, MAX_OPTION, flags);
			if (fType == TYPE_DIR&&iPass == 2 && recursive) {
				printf("Down To %s\n", findData.cFileName);
				SetCurrentDirectory(findData.cFileName);
				TraverseDirectory(FileName, DirName, numFlags, flags);
				SetCurrentDirectory(_T(".."));
			}
		} while (FindNextFile(searchHandle, &findData));
		FindClose(searchHandle);
	}
	return TRUE;
}
BOOL ProcessItem(LPWIN32_FIND_DATA pFileData, DWORD numFlags, LPBOOL flags) {
	const char fileTypeChar[] = { ' ','d' };
	DWORD fType = FileType(pFileData);
	BOOL longList = flags[0];
	SYSTEMTIME lastWrite;
	if (fType != TYPE_FILE && fType != TYPE_DIR) return FALSE;
	TCHAR currPath[MAX_PATH + 1];
	GetCurrentDirectory(MAX_PATH, currPath);
	printf("currPath %s\n", currPath);
	if (longList) {
		printf("%c ", fileTypeChar[fType - 1]);
		printf("%d ", pFileData->nFileSizeLow);
		FileTimeToSystemTime(&(pFileData->ftLastWriteTime), &lastWrite);
		printf("%d/ %d/ %d %d: %d: %d",
			lastWrite.wMonth, lastWrite.wDay,
			lastWrite.wYear, lastWrite.wHour,
			lastWrite.wMinute, lastWrite.wSecond);
	}
	printf(" %s\n", pFileData->cFileName);
	return TRUE;
}
DWORD FileType(LPWIN32_FIND_DATA pFileData) {
	BOOL isDir;
	DWORD fType;
	fType = TYPE_FILE;
	isDir = (pFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	if (isDir)
		if ((lstrcmp(pFileData->cFileName, _T("."))) == 0
			|| (lstrcmp(pFileData->cFileName, _T(".."))) == 0)
			fType = TYPE_DOT;
		else fType = TYPE_DIR;
		return fType;
}
BOOL TraverseRegistry(HKEY hKey, LPTSTR fullKeyName, LPTSTR subKey, LPBOOL flags) {
	HKEY hSubKey;
	BOOL recursive = TRUE;
	LONG result;
	DWORD valueType, index;
	DWORD numSubKeys, maxSubKeyLen, numValues, maxValueNameLen, maxValueLen;
	DWORD subKeyNameLen, valueNameLen, valueLen;
	FILETIME lastWriteTime;
	LPTSTR subKeyName, valueName;
	LPBYTE value;
	TCHAR fullSubKeyName[MAX_PATH + 1];
	//About RegOpenKeyEx()
	if (RegOpenKeyEx(hKey, subKey, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS)return FALSE;
	//About RegQueryInfoKey()
	if (RegQueryInfoKey(hSubKey, NULL, NULL, NULL, &numSubKeys, &maxSubKeyLen, NULL, &numValues, &maxValueNameLen, &maxValueLen,
		NULL, &lastWriteTime) != ERROR_SUCCESS)return FALSE;
	//malloc
	subKeyName = (LPTSTR)malloc(sizeof(char) * (maxSubKeyLen + 1));
	valueName = (LPTSTR)malloc(sizeof(char) * (maxValueNameLen + 1));
	value = (unsigned char*)malloc(maxValueLen);
	//Display All Pair
	for (index = 0; index < numValues; index++) {
		valueNameLen = maxValueNameLen + 1; //Both I/O see https://msdn.microsoft.com/en-us/library/windows/desktop/ms724865(v=vs.85).aspx
		valueLen = maxValueLen + 1;     //Like Above
										//About RegEnumValue()
		result = RegEnumValue(hSubKey, index, valueName, &valueNameLen, NULL,
			&valueType, value, &valueLen);
		if (result == ERROR_SUCCESS && GetLastError() == 0)
			DisplayPair(valueName, valueType, value, valueLen, flags);

	}
	//Recursively to All SubKey
	for (index = 0; index < numSubKeys; index++) {
		subKeyNameLen = maxSubKeyLen + 1;
		//About RegEnumKeyEx()
		result = RegEnumKeyEx(hSubKey, index, subKeyName, &subKeyNameLen, NULL,
			NULL, NULL, &lastWriteTime);
		if (GetLastError() == 0) {
			DisplaySubKey(fullKeyName, subKeyName, &lastWriteTime, flags);
			/*  Display subkey components if -R is specified */
			if (recursive) {
				//stprintf() 
				//similar to C++ stringstream
				_stprintf(fullSubKeyName, _T("%s\\%s"), fullKeyName, subKeyName);
				TraverseRegistry(hSubKey, fullSubKeyName, subKeyName, flags);
			}
		}
	}
	printf("\n");
	//free 
	free(subKeyName);
	free(valueName);
	free(value);
	RegCloseKey(hSubKey);
	return TRUE;
}
BOOL DisplayPair(LPTSTR valueName, DWORD valueType, LPBYTE value, DWORD valueLen, LPBOOL flags) {
	LPBYTE pV = value;
	DWORD i;

	printf("\n%s = ", valueName);
	switch (valueType) {
	case REG_FULL_RESOURCE_DESCRIPTOR:
	case REG_BINARY:
		for (i = 0; i < valueLen; i++, pV++)
			printf(" %x", *pV);
		break;

	case REG_DWORD: {
		printf("%d", *(DWORD*)value);
		break;
	}

	case REG_EXPAND_SZ:
	case REG_MULTI_SZ:
	case REG_SZ: {
		printf("%s", (LPTSTR)value);
		break;
	}
	case REG_DWORD_BIG_ENDIAN:
	case REG_LINK:
	case REG_NONE:
	case REG_RESOURCE_LIST:
	default: {
		printf(" ** Cannot display value of type: %d. Exercise for reader\n", valueType);
		break;
	}
	}

	return TRUE;
}
BOOL DisplaySubKey(LPTSTR keyName, LPTSTR subKeyName, PFILETIME pLastWrite, LPBOOL flags) {
	BOOL longList = FALSE;
	SYSTEMTIME sysLastWrite;

	printf("\n%s", keyName);
	if (_tcslen(subKeyName) > 0) printf("\\%s ", subKeyName);
	if (longList) {
		FileTimeToSystemTime(pLastWrite, &sysLastWrite);
		printf(" %02d/%02d/%04d %02d:%02d:%02d",
			sysLastWrite.wMonth, sysLastWrite.wDay,
			sysLastWrite.wYear, sysLastWrite.wHour,
			sysLastWrite.wMinute, sysLastWrite.wSecond);
	}
	return TRUE;
}
void ReportException(LPCWSTR userMessage, DWORD exceptionCode) {
	ReportError(userMessage, 0, TRUE);
	if (exceptionCode != 0) {
		RaiseException((0x0FFFFFFF)&exceptionCode | 0xE0000000, 0, 0, NULL);
	}
}
DWORD ErrorFilter(LPEXCEPTION_POINTERS pExP, LPDWORD eCategory) {
	DWORD exCode;
	DWORD_PTR readWrite, virtAddr;
	exCode = pExP->ExceptionRecord->ExceptionCode;
	printf("Filter. exCode: %x\n", exCode);
	if ((0x20000000 & exCode) != 0) {
		*eCategory = 10;
		return EXCEPTION_EXECUTE_HANDLER;
	}
	switch(exCode) {
	case EXCEPTION_ACCESS_VIOLATION:
		readWrite =
			(DWORD)(pExP->ExceptionRecord->ExceptionInformation[0]);
		virtAddr =
			(DWORD)(pExP->ExceptionRecord->ExceptionInformation[1]);
		printf("Access Violation. Read/Write/Execute: %d. Address: %x\n",
			readWrite, virtAddr);
		*eCategory = 1;
		return EXCEPTION_EXECUTE_HANDLER;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		*eCategory = 1;
		return EXCEPTION_EXECUTE_HANDLER;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
	case EXCEPTION_INT_OVERFLOW:
		*eCategory = 2;
		return EXCEPTION_EXECUTE_HANDLER;
	case 0xc00002b5:
	{   *eCategory = 3;
	printf("Flt Exception - Divide by Zero.\n");
	return EXCEPTION_EXECUTE_HANDLER; }
	case EXCEPTION_FLT_OVERFLOW:
		printf("Flt Exception - Large result.\n");
		*eCategory = 3;
		return EXCEPTION_EXECUTE_HANDLER;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
	case EXCEPTION_FLT_INEXACT_RESULT:
	case EXCEPTION_FLT_INVALID_OPERATION:
	case EXCEPTION_FLT_STACK_CHECK:
		printf("Flt Exception - Unknown result.\n");
		*eCategory = 3;
		return EXCEPTION_CONTINUE_EXECUTION;
	case EXCEPTION_FLT_UNDERFLOW:
		printf("Flt Exception - Small result.\n");
		*eCategory = 3;
		return EXCEPTION_CONTINUE_EXECUTION;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		*eCategory = 4;
		return EXCEPTION_CONTINUE_SEARCH;
	case STATUS_NONCONTINUABLE_EXCEPTION:
		*eCategory = 5;
		return EXCEPTION_EXECUTE_HANDLER;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
	case EXCEPTION_PRIV_INSTRUCTION:
		*eCategory = 6;
		return EXCEPTION_EXECUTE_HANDLER;
	case STATUS_NO_MEMORY:
		*eCategory = 7;
		return EXCEPTION_EXECUTE_HANDLER;
	default:
		*eCategory = 0;
		return EXCEPTION_CONTINUE_SEARCH;
	}
}


LPTNODE FillTree(HANDLE, HANDLE, HANDLE) {
	return nullptr;
}
BOOL InsertTree(LPPTNODE, LPTNODE) {

}
BOOL Scan(LPTNODE pNode) {
	if (pNode == NULL)
		return TRUE;
	Scan(pNode->Left);
    printf("%s\n", pNode->pData);
	Scan(pNode->Right);
	return TRUE;
}
int KeyCompare(LPCSTR pKey1, LPCSTR pKey2) {
	return strcmp(pKey1, pKey2);
}