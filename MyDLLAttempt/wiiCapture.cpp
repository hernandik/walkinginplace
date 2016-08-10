#include "stdafx.h"
#include <string>
#include <math.h>
#include <fstream>
#include <conio.h>
#include <vector>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "stdafx.h"
#include "windows.h"
#include "wiiCap.h"

#define MYMENU_EXIT         (WM_APP + 101)
#define MYMENU_MESSAGEBOX   (WM_APP + 102) 

HINSTANCE  inj_hModule;          //Injected Modules Handle
HWND       prnt_hWnd;            //Parent Window Handle
HWND		hwnd;

struct sysCfg *S;


#ifdef ENABLEWII
wii *Wii;
std::vector<wiimote *> rData;
#endif

double cmFps;
double mainFps;

HANDLE hHandleConectionMonitor;
DWORD ThreadIdConectionMonitor;
bool connectionMonitorStatus;
bool killBalCheck;
HANDLE ghMutexWiiDelete;

DWORD WINAPI CheckConnectionThreadRoutine(LPVOID lpArg);

//WndProc for the new window
LRESULT CALLBACK DLLWindowProc (HWND, UINT, WPARAM, LPARAM);

void on_state_change (wiimote			  &remote,
	state_change_flags  changed,
	const wiimote_state &new_state);

bool inited=false;

//Register our windows Class
BOOL RegisterDLLWindowClass(wchar_t szClassName[])
{
    WNDCLASSEXW wc;
    wc.hInstance =  inj_hModule;
    wc.lpszClassName = (LPCWSTR)szClassName;
    wc.lpfnWndProc = DLLWindowProc;
    wc.style =  CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    wc.cbSize = sizeof (WNDCLASSEX);
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    if (!RegisterClassExW (&wc))
		return 0;
}

//Creating our windows Menu
HMENU CreateDLLWindowMenu()
{
	HMENU hMenu;
	hMenu = CreateMenu();
	HMENU hMenuPopup;
    if(hMenu==NULL)
        return FALSE;
    hMenuPopup = CreatePopupMenu();
	AppendMenu (hMenuPopup, MF_STRING, MYMENU_EXIT, TEXT("Exit"));
    AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hMenuPopup, TEXT("File")); 

	hMenuPopup = CreatePopupMenu();
    AppendMenu (hMenuPopup, MF_STRING,MYMENU_MESSAGEBOX, TEXT("MessageBox")); 
    AppendMenu (hMenu, MF_POPUP, (UINT_PTR) hMenuPopup, TEXT("Test 2")); 
	return hMenu;
}

//The new thread
DWORD WINAPI MainThreadProc( LPVOID lpParam )
{
    MSG messages;
	
	wchar_t *pString = reinterpret_cast<wchar_t * > (lpParam);
	struct sendData *sensorData;
	sensorData = new struct sendData;
#ifdef ENABLEWII
	Wii = new wii();
	memset(sensorData, 0, sizeof(struct sendData));
	Wii->setInput1(sensorData->sensor);
	pClock(); // tem que vir antes da primeira chamada de getClockms, para carregar frequencia
	double clockI, clockIAnt;
	clockIAnt = getClockms();
#endif

	inited = true;
	killBalCheck = false;
	bool done = false;
	mainFps =	0.0;
	cmFps =		0.0;
	deltaT =	100;
	deltaS =	1;
	velocidade = 100;
	int p=0;
	float segundos=0;

	while(!done) {
#ifdef ENABLEWII
			clockI = getClockms();	
			clockIAnt = clockI;
			
			while((clockI - clockIAnt) < 0.01)
			{
				clockI = getClockms();
			}
			
			segundos += clockI-clockIAnt;
			mainFps = mix(mainFps, clockI - clockIAnt, 0.1f);
			p++;
			int i = 0;
			for(std::vector<wiimote *>::iterator iter = rData.begin(); iter != rData.end(); ++iter) {
				if((*iter)->RefreshState() != NO_CHANGE) {
					// copy data to final struct
					switch((*iter)->ExtensionType)
					{
					case wiimote_state::BALANCE_BOARD:
						{
							memcpy(&sensorData->sensor[i],
								&(*iter)->BalanceBoard.Kg,
								sizeof(struct sensors_f));
						}
						break;
					}
				}
				i++;	// tem que ficar fora para manter os dados estaveis nos seus devidos locais na estrutura
				sensorData->n = i;
			}

			Wii->nBalConnectadas = i;
			Wii->proc4bb();
#endif
	}
    return 1;
}

//Our new windows proc
LRESULT CALLBACK DLLWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PCOPYDATASTRUCT pMyCDS;

	switch (message)
    {
		case WM_COPYDATA:
		{
			pMyCDS = (PCOPYDATASTRUCT) lParam;
			switch( pMyCDS->dwData ){
			case MSG_WIIMOTE1:
				// Desativado, agora as mensagens da balanca sao capturadas por uma thread interna do programa
				// 30/9/2014
				// memcpy( &sensorData, (LPSTR) pMyCDS->lpData, sizeof(struct sendData));
				return TRUE;
			}
			break;
		}
		case WM_COMMAND:
               switch(wParam)
               {
                    case MYMENU_EXIT:
						ShowWindow(hwnd, SW_HIDE);
                        break;
                    case MYMENU_MESSAGEBOX:
						// para teste da thread e de SendMessage interno
						MessageBoxW(hwnd, L"Test", L"MessageBox",MB_OK);
                        break;
               }
               break;
		case WM_DESTROY:
			PostQuitMessage (0);
			break;
		default:
			return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

/**
 * Start thread to Monitor the capture routine for data capture routine
 */
void startConectionMonitor()
{

	ghMutexWiiDelete = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        "WiiDeleteWait");  // unnamed mutex

	if (ghMutexWiiDelete == NULL) 
		MessageBox(S->hWnd, "::CreateMutex Error", "ERROR", MB_OK);
	
	connectionMonitorStatus = true;
	hHandleConectionMonitor = CreateThread(NULL,0,MonitorConnectionsThreadRoutine, NULL,0, &ThreadIdConectionMonitor);
}

/*
 * This is the main loop that keep trying to receive data
 */
DWORD WINAPI MonitorConnectionsThreadRoutine(LPVOID lpArg)
{
	wiimote *wiiMoteTmp;
	wiiMoteTmp = new wiimote;
	connectionMonitorStatus = true; // sinalizador simples
	char *msg = new char[512];

	double cmClock, cmClockAnt;
	cmClock = getClockms();
	
	cmFps = 0;
	int n = 0;
	do{
		cmClockAnt = cmClock;
		cmClock = getClockms();

		cmFps = mix(cmFps, cmClock-cmClockAnt, 0.01f);

		if(wiiMoteTmp->Connect(wiimote::FIRST_AVAILABLE)) {
			sprintf(msg, "Balanca reconhecida, extension type %d (5 é Balança)\n", wiiMoteTmp->ExtensionType);
			writeLogger(msg);

			if(wiiMoteTmp->IsBalanceBoard()){
				wiiMoteTmp->SetLEDs(0x0f);
				wiiMoteTmp->SetReportType(wiimote::IN_BUTTONS_BALANCE_BOARD);
				DWORD dwWaitResult;
				dwWaitResult = WaitForSingleObject( 
					ghMutexWiiDelete,    // handle to mutex
					INFINITE);
				switch (dwWaitResult) {	 // The thread got ownership of the mutex
					case WAIT_OBJECT_0:
						rData.push_back(wiiMoteTmp);	// from this point, a new device was added!
						ReleaseMutex(ghMutexWiiDelete);
						break;
				}
				wiiMoteTmp = new wiimote;
				continue;
				// continue;
			}
			
			if(!wiiMoteTmp->IsBalanceBoard()) {
				// controle normal, armazena info na estrutura 5 neste caso
				wiiMoteTmp->SetLEDs(0x01);
				wiiMoteTmp->ChangedCallback		= on_state_change;
				//  notify us only when the wiimote connected sucessfully, or something
				//   related to extensions changes
				wiiMoteTmp->CallbackTriggerFlags = (state_change_flags)(CONNECTED |
					EXTENSION_CHANGED |
					MOTIONPLUS_CHANGED);
				wiiMoteTmp = new wiimote;
				continue;
			}
		}

		for(std::vector<wiimote *>::iterator iter = rData.begin(); iter != rData.end(); ) { // ++iter
			if((*iter)->ConnectionLost()) {
				sprintf(msg, "Uma balanca foi desconectada, cheque a LUZ da que desconectou (Bateria acabou?)\n");
				writeLogger(msg);
				// deleta item, mas antes lock recurso
				DWORD dwWaitResult;
				dwWaitResult = WaitForSingleObject( 
					ghMutexWiiDelete,    // handle to mutex
					INFINITE);
				std::vector<wiimote *>::iterator iterDeletado = iter;
				iter++;
				switch (dwWaitResult) {	 // The thread got ownership of the mutex
					case WAIT_OBJECT_0:
						rData.erase(iterDeletado);
						ReleaseMutex(ghMutexWiiDelete);
						break;
				}
			} else {
				iter++;
			}
		}

		if(killBalCheck) {
			sprintf(msg, "Fechando Thread que gerencia a conexão das balanças\n");
			writeLogger(msg);
			return true;
		}
	}while(connectionMonitorStatus); // matar a thread aqui

	// desconecta todos os dispositivos
	DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject( 
			ghMutexWiiDelete,    // handle to mutex
			INFINITE);
	switch (dwWaitResult)	 // The thread got ownership of the mutex
	{
		case WAIT_OBJECT_0:
			for(std::vector<wiimote *>::iterator iter = rData.begin(); iter != rData.end(); ++iter) {
				(*iter)->Disconnect(); // ConnectionLost())
				rData.erase(iter);
				if(rData.size()== 0)
					break;
			}
			break;
	}
	ReleaseMutex(ghMutexWiiDelete);
	return TRUE;
}

void on_state_change (wiimote			  &remote,
	state_change_flags  changed,
	const wiimote_state &new_state)
{
	// we use this callback to set report types etc. to respond to key events
	//  (like the wiimote connecting or extensions (dis)connecting).

	// NOTE: don't access the public state from the 'remote' object here, as it will
	//		  be out-of-date (it's only updated via RefreshState() calls, and these
	//		  are reserved for the main application so it can be sure the values
	//		  stay consistent between calls).  Instead query 'new_state' only.

	// the wiimote just connected
	if(changed & CONNECTED)
	{
		// ask the wiimote to report everything (using the 'non-continous updates'
		//  default mode - updates will be frequent anyway due to the acceleration/IR
		//  values changing):

		// note1: you don't need to set a report type for Balance Boards - the
		//		   library does it automatically.

		// note2: for wiimotes, the report mode that includes the extension data
		//		   unfortunately only reports the 'BASIC' IR info (ie. no dot sizes),
		//		   so let's choose the best mode based on the extension status:
		printf("!!!!!!!!!!!!!!!!!!!!!EVENT");
		if(new_state.ExtensionType != wiimote::BALANCE_BOARD)
		{
			if(new_state.bExtension)
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
			else
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);		//    IR dots
		}
	}
	// a MotionPlus was detected
	if(changed & MOTIONPLUS_DETECTED)
	{
		printf("!!!!!!!!!!!!!!!!!!!!! OTHER EVENT");
		// enable it if there isn't a normal extension plugged into it
		// (MotionPlus devices don't report like normal extensions until
		//  enabled - and then, other extensions attached to it will no longer be
		//  reported (so disable the M+ when you want to access them again).
		if(remote.ExtensionType == wiimote_state::NONE) {
			bool res = remote.EnableMotionPlus();
			_ASSERT(res);
		}
	}
	// an extension is connected to the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_CONNECTED)
	{
		printf("!!!!!!!!!!!!!!!!!!!!! YET OTHER EVENT");
		// We can't read it if the MotionPlus is currently enabled, so disable it:
		if(remote.MotionPlusEnabled())
			remote.DisableMotionPlus();
	}
	// an extension disconnected from the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_DISCONNECTED)
	{
		printf("!!!!!!!!!!!!!!!!!!!!! AND OTHER EVENT");
		// enable the MotionPlus data again:
		if(remote.MotionPlusConnected())
			remote.EnableMotionPlus();
	}
	// another extension was just connected:
	else if(changed & EXTENSION_CONNECTED)
	{
		printf("!!!!!!!!!!!!!!!!!!!!! AND ANOTHER EVENT");
		// switch to a report mode that includes the extension data (we will
		//  loose the IR dot sizes)
		// note: there is no need to set report types for a Balance Board.
		if(!remote.IsBalanceBoard())
			remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);
	}
	// extension was just disconnected:
	else if(changed & EXTENSION_DISCONNECTED)
	{
		printf("!!!!!!!!!!!!!!!!!!!!! AND LAST EVENT");
		// use a non-extension report mode (this gives us back the IR dot sizes)
		remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
	}
}

void writeLogger(char *i) {
	if((strlen(logger)+strlen(i)) > 1023)
	{
		// overwrite logger
		sprintf(logger, "%s",i);
	}else {
		sprintf(logger, "%s%s", logger,i);
	}
}

extern "C"
{
__declspec(dllexport) void enableCorrectBalPosition(int i)
{
	if(Wii->nBalConnectadas==0)
		return;
	Wii->startAutomatedFixBalPosition();
	return;
}

__declspec(dllexport) void continueFlag() {

	Wii->continueFlag();
}

__declspec(dllexport) void setDirectionRef()
{
	// capture and save the direction vector
	return;
}

__declspec(dllexport) void killBallCheck()
{
	killBalCheck = true;
}

__declspec(dllexport) void getVirtualDirection(vec3f *a) {
	*a = Wii->virtualDirection;
	return;
}

__declspec(dllexport) void getSensorData(struct sendData *sD)
{
	// ajustado para pegar os dados das balanças alinhados
	if(!inited)	// nao inicializado ainda
		return;
	Wii->getFixedSensorData(sD->sensor);
}

__declspec(dllexport) void getFPS(double *aa)
{
	aa[0] = mainFps;
	aa[1] = cmFps;
	return;
}

__declspec(dllexport) void openWIP(){
}

__declspec(dllexport) void getLog(char *aaa) {
	strcpy(aaa, logger);
}

__declspec(dllexport) void getLogW(WCHAR *aaa) {
	swprintf(aaa, L"%s", logger);
}

// a deve conter 4 floats
__declspec(dllexport) void getOrientation(float *a){

#ifdef ENABLEWII
	a[0] = Wii->refVertex.x;
	a[1] = Wii->refVertex.y;
	a[2] = Wii->refVertex.z;
	a[3] = Wii->refVertex.w;
#endif
}

__declspec(dllexport) int init()
{
	ShowWindow(hwnd, SW_SHOW);
	return 555;
}

__declspec(dllexport) void confirmMsg()
{
	MessageBoxW(hwnd, L"Confirm Msg OK!",L"Aviso", MB_OK);
}

// a deve conter 4 floats
__declspec(dllexport) void getOriSpeed3f(float *b)
{
#ifdef ENABLEWII
	vec3f *a;
	a = (vec3f *) b;

	*a = Wii->refVertex;
	a++;
	*a = Wii->outSpeed;
#endif
}

__declspec(dllexport) void getSpeed(float *a)
{
#ifdef ENABLEWII
	a[0] = Wii->outSpeed.x;
	a[1] = Wii->outSpeed.y; //Wii->refVertex.y;
	a[2] = Wii->outSpeed.z; // Wii->refVertex.z;
	a[3] = Wii->outSpeed.w; // Wii->refVertex.w;
#endif
}

__declspec(dllexport) void disconectWIP()
{
	SendMessage(hwnd, WM_CLOSE, 0, 0);
}

}


