// Win32_MSVC.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <cstdio>
#include "Everything.h" 
#include "myHeader_V2.h" 
#include "float.h"
int main(int argc, LPCSTR argv[])
{
	DWORD eCategory, i = 0, ix, iy = 0;
	LPDWORD pNull = NULL;
	double x = 1, y = 0;
	BOOL done = FALSE;
	//set float point exception
	unsigned int fpOld, fpNew, fpOldDummy;
	_controlfp_s(&fpOld, 0, 0);
	fpNew = fpOld & ~(EM_OVERFLOW | EM_UNDERFLOW | EM_INEXACT
		| EM_ZERODIVIDE | EM_DENORMAL | EM_INVALID);
	_controlfp_s(&fpOldDummy, fpNew, MCW_EM);
	while (!done) {
		__try {
			printf("Enter exception type:\n");
			printf("1: Mem, 2: Int, 3: Flt 4: User 5: _leave 6: return\n");
			scanf_s("%d", &i);
			__try {
				switch (i) {
				case 1:
					ix = *pNull;
					*pNull = 5;
					break;
				case 2:
					ix = ix / iy;
					break;
				case 3:
					printf("y=0\n");
					x = x / y;
					break;
				case 4:
					ReportException(_T("Raising user exception.\n"), 1);
					break;
				case 5:
					done = TRUE;
					__leave;
				case 6:
					return 1;
				default: done = TRUE;
				}
			}
			__except (ErrorFilter(GetExceptionInformation(), &eCategory)) {
				switch (eCategory) {
				case 0:	_tprintf(_T("Unknown exception.\n"));
					break;
				case 1:	_tprintf(_T("Memory ref exception.\n"));
					break;
				case 2:	_tprintf(_T("Integer arithmetic exception.\n"));
					break;
				case 3:
					_tprintf(_T("floating-point exception.\n"));
					break;
				case 10: _tprintf(_T("User generated exception.\n"));
					break;
				default: _tprintf(_T("Unknown exception.\n"));
					break;
				}
				_tprintf(_T("End of handler.\n"));
			}
		}
		__finally {
			BOOL AbTerm;
			_controlfp_s(&fpOldDummy, fpOld, MCW_EM);
			AbTerm = AbnormalTermination();
			_tprintf(_T("Abnormal Termination?: %d\n"), AbTerm);
		}
		return 0;
	}
}


