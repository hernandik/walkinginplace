#ifndef _DERIVADA
#define _DERIVADA

//#include "chico.h"
//#include "vetoriais.h"
//#include "..\..\libs\lowPassFilter.h"
//#include "lineObj.h"
//#include "stdafx.h"
#include "vetorial.h"

class derivada{
	//lowPass *filtroA;
	//lowPass *filtroT;

	int rP;
	int wP;
	int pTotal;
	int p;

	vec3f *a;//[2];
	long long *t;//[2];
	double *tDouble;
	//bool p;
	//lineObj *l;
public:
	vec3f der;
	derivada();
	void show();
	void add(vec3f *xi, long long ti);
	void add(vec3f *xi, double ti);
};

#endif
