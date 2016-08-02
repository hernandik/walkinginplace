#pragma once

#include	<windows.h>
//#include	"glext.h"
#include	<iostream>
#include	<vector>
#include	<set>
#include	"vetorial.h"

void gotoxys(int x, int y);
vec3f mix(vec3f a,vec3f b, float s);
vector4f mix(vector4f a, vector4f b, float s);
double getClockms();
void pClock();
void StartCounter();
float findSizeV(vec3f *a, vec3f *b, int n);
float findSize(vec3f *in, vec3f *sizeOut, vec3f *minOut, int n);
void swap(float &a, float &b);

inline float mix(float a, float b, float x)
{
	return a*(1.0f-x) + (b * x);
};

inline double mix(double a, double b, float x)
{
	return a*(1.0f-x) + (b * x);
};

inline float clamp(float f, float min, float max)
{
	if(f > max)
		return max;
	if (f < min)
		return min;
	return f;
};

extern double clock;
extern double clockAnterior;
extern double deltaT;
extern double deltaS;
extern float velocidade;
