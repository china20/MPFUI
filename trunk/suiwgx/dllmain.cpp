// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

HMODULE suiwgx_handle = NULL;

#ifndef SUIWGX_LIB

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    suiwgx_handle = hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        DllRegisterServer();
        break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif
