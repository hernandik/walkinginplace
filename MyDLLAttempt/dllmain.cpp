// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "wiiCap.h"

char *logger;
// struct sysCfg *S;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		S = new struct sysCfg;
		inj_hModule = hModule;		//S->hInstance = hModule; // 
		// MessageBoxW(0, L"DLL loaded", L"Aviso", MB_OK);
		inited=false;
		
		logger = new char[2048];
		ZeroMemory(logger, 2048);

		CreateThread(0, NULL, MainThreadProc, (LPVOID)L"WIP Monitor", NULL, NULL);
		
		// monitor de conexao de balancas
		connectionMonitorStatus = false;
		startConectionMonitor();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		// delete[] logger;
		// SendMessage(hwnd, WM_CLOSE, 0, 0);
		break;
	}
	return TRUE;
}

