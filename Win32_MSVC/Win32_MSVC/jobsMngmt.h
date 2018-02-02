#pragma once
#include "Everything.h" 
LONG GetJobNumber(PROCESS_INFORMATION *pProcessInfo, LPCTSTR Command);
BOOL DisplayJobs(void);
DWORD FindProcessId(DWORD jobNumber);