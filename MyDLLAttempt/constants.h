#ifndef _CONSTANTS
#define _CONSTANTS

#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// #include "..\glew-1.9.0\include\GL\glew.h"

#define ij(a,b) ((a)*3)+b
#define ijn(a,b,n) ((a)*(n))+b
#define showV(a) printf("%f %f %f\n", (a).x, (a).y, (a).z)

#define IsAlign(a) ((unsigned long)(a) & 0xf)
//#define IsAlign(a) (((*UINT)(const void *)(a)) & 0xf)

const float piover180 = (float)(M_PI/180.0f);

typedef unsigned short USHORT;
typedef unsigned int UINT;

class SETOR;
class colision;
class colorADE;
class hgt;
class matrix4x4f;

//class CAM;

void gotoxys(int x, int y);
void swap(float &a, float &b);




#endif