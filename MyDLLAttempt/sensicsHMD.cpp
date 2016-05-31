#include "stdafx.h"
#include <string>
#include <Dinput.h>
#include <math.h>
#include <fstream>
#include <conio.h>
#include <atlstr.h>

/* Part of this code is of propriety of sensics */
extern "C"
{

CComPtr<IDirectInput8> m_pDI=NULL;
CComPtr<IDirectInputDevice8> m_pSensics=NULL;

BOOL CALLBACK EnumSensicsCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
	
BOOL CALLBACK EnumSensicsCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
	HRESULT hr = m_pDI->CreateDevice(pdidInstance->guidInstance, &m_pSensics, NULL );
	if (SUCCEEDED(hr) == TRUE)
	{
		if(wcscmp(L"Sensics zSight HMD",pdidInstance->tszProductName)!=0)
			m_pSensics.Release();
		else
			return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
}

BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
	if (pdidoi->dwType & DIDFT_AXIS)
	{
		DIPROPRANGE diprg;
		::memset(&diprg,0,sizeof diprg);
		diprg.diph.dwSize = sizeof(DIPROPRANGE);
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		diprg.diph.dwHow = DIPH_BYID;
		diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
		diprg.lMin = 0;
		diprg.lMax = 65535;
		// Set the range for the axis
		if( FAILED( m_pSensics->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
			return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
}


static short fromTwos( unsigned short x )
{
	if ( x < 0x8000 )
	{
		return (short) x;
	}
	else
	{
		x = x ^ 0xFFFF;
		short y = (short) x;
		if ( y != 32767 )
		{
			y = y + 1;
		}
		y = -y;
		return (short) y;
	}
}

static void quaternionToMatrix( float q[4], float m[3][3] )
{
	m[0][0] = q[3] * q[3] + q[0] * q[0] - q[1] * q[1] - q[2] * q[2];
	m[0][1] = 2 * (q[0] * q[1] + q[2] * q[3]);
	m[0][2] = 2 * (q[0] * q[2] - q[1] * q[3]);
	m[1][0] = 2 * (q[0] * q[1] - q[2] * q[3]);
	m[1][1] = q[3] * q[3] - q[0] * q[0] + q[1] * q[1] - q[2] * q[2];
	m[1][2] = 2 * (q[1] * q[2] + q[0] * q[3]);
	m[2][0] = 2 * (q[0] * q[2] + q[1] * q[3]);
	m[2][1] = 2 * (q[1] * q[2] - q[0] * q[3]);
	m[2][2] = q[3] * q[3] - q[0] * q[0] - q[1] * q[1] + q[2] * q[2];
}

static void matrixToYPR( const float m[3][3], float ypr[3] )
{
	ypr[0] = atan2( m[0][1], m[0][0] ); // yaw
	ypr[1] = asin( m[0][2] ); // pitch
	ypr[2] = -atan2( m[1][2], m[2][2] ); // roll
}

__declspec(dllexport) void closeSensicsHMD()
{
	if (m_pSensics)
	{
		m_pSensics->Unacquire();
	}
}

__declspec(dllexport) bool openSensicsHMD()
{
	if (m_pSensics)
	{
		m_pSensics->Unacquire();
	}

	if ( (::DirectInput8Create(::GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pDI,
		NULL )) !=  DI_OK)
	{
		return false;
	}

	m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumSensicsCallback, 0, DIEDFL_ATTACHEDONLY);
	if (m_pSensics == 0)
		return false;

	if ( m_pSensics->SetDataFormat( &c_dfDIJoystick2 ) != DI_OK)
	{
		m_pSensics.Release();
		return false;
	}

	m_pSensics->SetCooperativeLevel(0, DISCL_EXCLUSIVE|DISCL_FOREGROUND );
	m_pSensics->EnumObjects(EnumObjectsCallback,0,DIDFT_AXIS);
	m_pSensics->Acquire();

	return true;
}

__declspec(dllexport) bool getDataSensicsHMD(float* data)
{
	data[0] = -1.0f;
	data[1] = -1.0f;
	data[2] = -1.0f;
	data[3] = -1.0f;

	DIJOYSTATE2 js; // DirectInput joystick state structure
	if(m_pSensics->GetDeviceState( sizeof(js), &js )==  DI_OK)
	{
		data[0] = fromTwos( (unsigned short) js.lRx ) / 32768.0f;
		data[1] = fromTwos( (unsigned short) js.lRy ) / 32768.0f;
		data[2] = fromTwos( (unsigned short) js.lX ) / 32768.0f;
		data[3] = fromTwos( (unsigned short) js.lY ) / 32768.0f;

		int x;;
		float norm = 0.0f;
		for(x=0;x<4;++x)
		{
			 norm += data[x]*data[x];
		}
		norm=sqrt(norm);
		

		for(x=0;x<4;++x)
		{
			 data[x]= data[x]/norm;
		}

		return true;
	}
	
	return false;
}

}