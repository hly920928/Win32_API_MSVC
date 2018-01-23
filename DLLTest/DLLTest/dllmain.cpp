// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <cstdio>
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		printf_s("Entry Point proAt\n"); break;}
	case DLL_THREAD_ATTACH: {
		printf_s("Entry Point thrAt\n"); break; }
	case DLL_THREAD_DETACH: {
		printf_s("Entry Point thrDet\n"); break; }
	case DLL_PROCESS_DETACH:{
		printf_s("Entry Point proDet\n"); break;}
	}
	return TRUE;
}

