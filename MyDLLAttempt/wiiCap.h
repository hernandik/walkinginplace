#pragma once

#include "super_Basic.h"
#include "wiimote.h"
#include "psapi.h"
#include "wiiBalBoard.h"

#define ENABLEWII

typedef struct sysCfg
{
	HGLRC		hRC1;					// Permanent Rendering Context
	HGLRC		hRC2;					// RC for texture loading and others things
	HDC			hDC;					// Private GDI Device Context
	HWND		hWnd;					// Holds Our Window Handle
	HINSTANCE	hInstance;				// Holds The Instance Of The Application
	bool	fullscreen;					// Fullscreen Flag Set To Fullscreen Mode By Default
	int scrWidth, scrHeight;
	int cbits; // bits de cor
} SYSCFG;

struct remoteData{
	wiimote *remote;
	bool isEnable;
	int id;
	struct sensors_f *sensors;
	char *message;
};


extern struct sysCfg *S;
extern HINSTANCE  inj_hModule;          //Injected Modules Handle
extern HWND		hwnd;
extern bool connectionMonitorStatus;
extern char *logger;
extern bool inited;

#define MSG_WIIMOTE1 0x8010

void startConectionMonitor();
DWORD WINAPI MainThreadProc( LPVOID lpParam );
DWORD WINAPI MonitorConnectionsThreadRoutine(LPVOID lpArg);
void writeLogger(char *i);
