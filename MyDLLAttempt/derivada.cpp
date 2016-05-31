//#include "main.h"
#include "stdafx.h"
#include "derivada.h"

//extern FILE *logs;

derivada::derivada()
{
	pTotal = 24;
	a = new vec3f[pTotal];
	t = new long long[pTotal];
	tDouble = new double[pTotal];

	rP = 0;
	wP = 0;

	//l = new lineObj(200, false);
	//l->cor.set(1,0,0,1);

	p = 0;
}

void derivada::show()
{
	//l->show(vec3f(1.3f,1.7f,0.0f));
}

void derivada::add(vec3f *xi, double ti)
{
	a[wP] = *xi;
	tDouble[wP] = ti;

	wP++;
	wP = wP % pTotal;

	// contador de itens no HIVE
	p++;
	if(p > pTotal ) p = pTotal;

	int halfHive = p/2;

	if(halfHive < 2)
	{
		// printf("der: HALFHIVE %d\n", halfHive);
		return;
	}

	vec3f da, db;
	float ta=0, tb=0;
	int pa, pb;
	for(int i = 0; i < halfHive; i++)
	{
		//printf(" %d %d ", (wP+i) % pTotal, (wP+i+halfHive) % pTotal);
		pa = (wP+i) % pTotal;
		pb = (wP+i+halfHive) % pTotal;
		// vetor
		da += a[pa];
		db += a[pb];
		// tempo
		ta += tDouble[pa];
		tb += tDouble[pb];
	}

	ta = (tb - ta)/ (float) halfHive;
	da = (db - da)/ (float) halfHive;
	
	//da = da / ta;
	//da = da * (1/(float) halfHive) * (ta * (1/(float) halfHive));
	//db = db * (1/(float) halfHive);
	der = (da / ta)/10.0;	///ta;// - db;
	der = (da);// / ta)/10.0;	///ta;// - db;
}

void derivada::add(vec3f *xi, long long ti){
	a[wP] = *xi;
	t[wP] = ti;

	wP++;
	wP = wP % pTotal;

	// contador de itens no HIVE
	p++;
	if(p > pTotal ) p = pTotal;

	int halfHive = p/2;

	if(halfHive < 2)
	{
		//printf("der: HALFHIVE %d\n", halfHive);
		return;
	}

	vec3f da, db;
	float ta=0, tb=0;
	int pa, pb;
	for(int i = 0; i < halfHive; i++)
	{
		//printf(" %d %d ", (wP+i) % pTotal, (wP+i+halfHive) % pTotal);
		pa = (wP+i) % pTotal;
		pb = (wP+i+halfHive) % pTotal;
		// vetor
		da += a[pa];
		db += a[pb];
		// tempo
		ta += t[pa];
		tb += t[pb];
	}

	ta = (tb - ta)/ (float) halfHive;
	da = (db - da)/ (float) halfHive;
	
	//da = da / ta;
	//da = da * (1/(float) halfHive) * (ta * (1/(float) halfHive));
	//db = db * (1/(float) halfHive);
	der = da * ta;	///ta;// - db;
}


