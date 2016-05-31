#include "stdafx.h"

#include "super_Basic.h"

double clock=0;
double clockAnterior=0;
__int64 PCFreq = 0l;

double deltaT=100;
double deltaS=100;

float velocidade=250;
float timeAcum=0;

void gotoxys(int x, int y)
{
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

#ifdef linux
void gotoxys(int x, int y)
{
    printf("%c[%d;%df",0x1B,y,x);
}

inline void ZeroMemory(void *a, unsigned int s)
{
	memset(a,NULL, s);
}
#endif

vec3f mix(vec3f a,vec3f b, float s)
{
	vec3f o;
	o = (a*(1-s))+(b*s);
	return o;
}

vector4f mix(vector4f a, vector4f b, float s)
{
	vector4f o;
	o = (a*(1-s))+(b*s);
	return o;
}

void swap(float &a, float &b)
{
	float c;
	c = a;
	a = b;
	b = c;
}

void StartCounter()
{
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
		MessageBox(NULL, "Query Performance Frequency Failed (are you using an old computer?)", "Aviso", MB_OK);
		// printf("QueryPerformanceFrequency failed!\n");
	
	PCFreq = li.QuadPart;///1000.0;

	// printf("Initiating Timer\nPerformance Counter %lld hz\n", PCFreq);

    QueryPerformanceCounter(&li);
    clockAnterior = clock = li.QuadPart / (double) PCFreq;
	// printf("Initiating Timer: clock Inicial %lld\n", clockAnterior);
}

void pClock()
{
	static bool firstTime=0;

	if(firstTime==0)
	{
		StartCounter();
		firstTime=1;
	}

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	clockAnterior = clock;
	clock = li.QuadPart / (double) PCFreq;

	deltaT = (clock-clockAnterior);///(double)PCFreq);
	//GetTickCount();
	deltaS = deltaT * velocidade; // 7ms estimativa

	if(deltaT != deltaT)
	{
		// printf("timerPasso NAN");
		deltaT = 1;
	}
}

double getClockms()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return li.QuadPart / (double) PCFreq;
}


float findSize(vec3f *in, vec3f *sizeOut, vec3f *minOut, int n)
{
	vec3f max, min;
	max = in[0];
	min = in[0];
	for(int i = 0; i < n; i++)
	{
		if(in[i].x > max.x) max.x = in[i].x;
		else if(in[i].x <= min.x) min.x = in[i].x;

		if(in[i].y > max.y) max.y = in[i].y;
		else if(in[i].y <= min.y) min.y = in[i].y;

		if(in[i].z > max.z) max.z = in[i].z;
		else if(in[i].z <= min.z) min.z = in[i].z;
	}

	*minOut = min;
	*sizeOut = (max - min);

	if(sizeOut->x > sizeOut->y && sizeOut->x > sizeOut->z)
		return sizeOut->x;
	else if (sizeOut->y > sizeOut->z)
		return sizeOut->y;
	else return sizeOut->z;
}

float findSizeV(vec3f *a, vec3f *b, int n)
{
	vec3f minOut;
	return findSize(a,b, &minOut, n);
}