#ifndef LOWPASSFILTER
#define LOWPASSFILTER

#include "vetorial.h"

class lowPass{
	int nLev;
	int pBuf;
	vec3f *c;
	int nItens;
	public:

		float aa;
		lowPass(int n);
		~lowPass();
		void update(vec3f *vIn);
		void updateG(vec3f *vIn);
		void updateH(vec3f *vIn);
		void updateIIR(vec3f *vIn);
	//void get();
};

#endif
