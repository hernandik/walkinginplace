#include "stdafx.h"
#include "lowPassFilter.h"


lowPass::lowPass(int n)
{
	nLev = n;
	c = new vec3f[n];
	pBuf = 0;
	nItens = 0;
}

lowPass::~lowPass()
{
	delete []c;
}

void lowPass::update(vec3f *vIn)
{
	c[pBuf] = *vIn;
	
	pBuf++;

	if(pBuf >= nLev)
		pBuf = 0;
	
	nItens++;
	if(nItens > nLev)
		nItens = nLev;

	// seta novo valor para vertice!
	vIn->set(0,0,0);
	
	for(int i=0; i < nItens; i++)
	{
		*vIn  = *vIn + c[i];
	}

	float frac;

	frac = 1 / (float) nItens;
	vIn->x *= frac;
	vIn->y *= frac;
	vIn->z *= frac;
}

void lowPass::updateG(vec3f *vIn)
{
	c[pBuf] = *vIn;
	
	pBuf++;

	if(pBuf >= nLev)
		pBuf = 0;
	
	nItens++;
	if(nItens > nLev)
		nItens = nLev;

	*vIn = c[0]; //->set(0,0,0);
	for(int i=1; i < nItens; i++)
	{
		*vIn  = *vIn * c[i];
	}

	vIn->x = pow(vIn->x,1/float(nItens));
	vIn->y = pow(vIn->y,1/float(nItens));
	vIn->z = pow(vIn->z,1/float(nItens));
}

void lowPass::updateH(vec3f *vIn)
{
	c[pBuf] = *vIn;
	
	pBuf++;

	if(pBuf >= nLev)
		pBuf = 0;
	
	nItens++;
	if(nItens > nLev)
		nItens = nLev;

	vIn->set(0,0,0);

	for(int i=0; i < nItens; i++)
	{
		*vIn  = *vIn + (1.0f/c[i]);
	}
	
	*vIn = nItens / *vIn;
}

void lowPass::updateIIR(vec3f *vIn)
{
	c[1] = *vIn;
	
	*vIn = (c[1] * aa ) + (c[0]* (1-aa));
	
	c[0] = *vIn;

}