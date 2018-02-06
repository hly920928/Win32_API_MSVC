#include "stdafx.h"
#include "helperFunction.h"

LPSTR SkipArg(LPSTR cLine, int argn, int argc, LPSTR argv[])
{
	LPSTR pArg = cLine, cEnd = pArg + strlen(cLine);
	int iArg;

	if (argn >= argc) return NULL;

	for (iArg = 0; iArg < argn && pArg < cEnd; iArg++)
	{
		if ('"' == *pArg)
		{
			pArg += strlen(argv[iArg]) + 2;
		}
		else
		{
			pArg += strlen(argv[iArg]);
		}

		while ((pArg < cEnd) && ((' ') == *pArg) || ('\t') == *pArg) pArg++;
	}

	if (pArg >= cEnd) return NULL;
	return pArg;
}

DWORD Options(int argc, LPSTR argv[], LPCSTR OptStr, ...)
{
	{
		va_list pFlagList;
		LPBOOL pFlag;
		int iFlag = 0, iArg;
		va_start(pFlagList, OptStr);
		while ((pFlag = va_arg(pFlagList, LPBOOL)) != NULL
			&& iFlag < (int)strlen(OptStr)) {
			*pFlag = FALSE;
			for (iArg = 1; !(*pFlag) && iArg < argc && argv[iArg][0] == '-'; iArg++)
				*pFlag = memchr(argv[iArg], OptStr[iFlag],
					strlen(argv[iArg])) != NULL;
			iFlag++;
		}
		va_end(pFlagList);
		for (iArg = 1; iArg < argc && argv[iArg][0] == '-'; iArg++);
		return iArg;
	}
}

void GetArgs(LPCSTR Command, DWORD * pArgc, LPSTR argstr[])
{
	int i, icm = 0;
	DWORD ic = 0;

	for (i = 0; ic < strlen(Command); i++) {
		while (ic < strlen(Command) &&
			Command[ic] != ' ' && Command[ic] != '\t') {
			argstr[i][icm] = Command[ic];
			ic++;
			icm++;
		}
		argstr[i][icm] = '\0';
		while (ic < strlen(Command) &&
			(Command[ic] == ' ' || Command[ic] == '\t'))
			ic++;
		icm = 0;
	}

	if (pArgc != NULL) *pArgc = i;
	return;
}
