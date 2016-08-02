//-----------------------------------------------------------------------------
//           Name: matrix4x4f.h
//         Author: Kevin Harris
//  Last Modified: March 13, 2005 by Manuel Menezes
//    Description: OpenGL compatible utility class for a 4x4 matrix of floats.
//                 NOTE: This class has been left unoptimized for readability.
//-----------------------------------------------------------------------------

#ifndef _MATRIX4X4F_H_
#define _MATRIX4X4F_H_

#include <stdio.h>
//#include "basic.h"
#include "constants.h"
#include "vetorial.h"
// #include "cam.h"
// #include "..\glew-1.9.0\include\GL\glew.h"
#include <pmmintrin.h>

#define DEGTORAD(degree) ((degree) * (3.141592654f / 180.0f))
#define RADTODEG(radian) ((radian) * (180.0f / 3.141592654f))

class matrix4x4f
{
public:
	
	__declspec(align(16)) float m[16];

    matrix4x4f()
	{
		//printf("matrix4x4f:: align this %d m %d\n", IsAlign(this), IsAlign(m));
		identity();
	}

    matrix4x4f( float m0, float m4, float  m8, float m12,
                float m1, float m5, float  m9, float m13,
                float m2, float m6, float m10, float m14,
                float m3, float m7, float m11, float m15 );

	void set( float m0, float m4, float  m8, float m12,
                float m1, float m5, float  m9, float m13,
                float m2, float m6, float m10, float m14,
                float m3, float m7, float m11, float m15 );

    void identity(void);
	void identitySlow(void);
	void identityFast( void );
	void transformVectorSetFast(vec3f *vec, int n);
	void transformVector( vector4f *v );

	matrix4x4f operator= (const matrix4x4f &aa)
	{
		for(int i=0; i<16; i++)
			m[i] = aa.m[i];

		//__m128 a, b, c, d;
		//a = _mm_load_ps(&aa.m[0]);
		//b = _mm_load_ps(&aa.m[4]);
		//c = _mm_load_ps(&aa.m[8]);
		//d = _mm_load_ps(&aa.m[12]);
		//_mm_store_ps(&m[0],a);
		//_mm_store_ps(&m[4],b);
		//_mm_store_ps(&m[8],c);
		//_mm_store_ps(&m[12],d);
		return *this;
	};

	void attSlow (const matrix4x4f &aa)
	{
		for(int i=0; i<16; i++)
			m[i] = aa.m[i];
	};



	matrix4x4f operator* (const matrix4x4f &b)
	{
		matrix4x4f c;

		c.mmul_sse_unroll(m, b.m); //, float * r)

		//c.m[0]= (this->m[0] * b.m[0]) + (this->m[4] * b.m[1]) + (this->m[8] * b.m[2]) + (this->m[12] * b.m[3]);
		//c.m[4]= (this->m[0] * b.m[4]) + (this->m[4] * b.m[5]) + (this->m[8] * b.m[6]) + (this->m[12] * b.m[7]);
		//c.m[8]= (this->m[0] * b.m[8]) + (this->m[4] * b.m[9]) + (this->m[8] * b.m[10]) + (this->m[12] * b.m[11]);
		//c.m[12]= (this->m[0] * b.m[12]) + (this->m[4] * b.m[13]) + (this->m[8] * b.m[14]) + (this->m[12] * b.m[15]);

		//c.m[1]= (this->m[1] * b.m[0]) + (this->m[5] * b.m[1]) + (this->m[9] * b.m[2]) + (this->m[13] * b.m[3]);
		//c.m[5]= (this->m[1] * b.m[4]) + (this->m[5] * b.m[5]) + (this->m[9] * b.m[6]) + (this->m[13] * b.m[7]);
		//c.m[9]= (this->m[1] * b.m[8]) + (this->m[5] * b.m[9]) + (this->m[9] * b.m[10]) + (this->m[13] * b.m[11]);
		//c.m[13]= (this->m[1] * b.m[12]) + (this->m[5] * b.m[13]) + (this->m[9] * b.m[14]) + (this->m[13] * b.m[15]);

		//c.m[2]= (this->m[2] * b.m[0]) + (this->m[6] * b.m[1]) + (this->m[10] * b.m[2]) + (this->m[14] * b.m[3]);
		//c.m[6]= (this->m[2] * b.m[4]) + (this->m[6] * b.m[5]) + (this->m[10] * b.m[6]) + (this->m[14] * b.m[7]);
		//c.m[10]= (this->m[2] * b.m[8]) + (this->m[6] * b.m[9]) + (this->m[10] * b.m[10]) + (this->m[14] * b.m[11]);
		//c.m[14]= (this->m[2] * b.m[12]) + (this->m[6] * b.m[13]) + (this->m[10] * b.m[14]) + (this->m[14] * b.m[15]);

		//c.m[3]= (this->m[3] * b.m[0]) + (this->m[7] * b.m[1]) + (this->m[11] * b.m[2]) + (this->m[15] * b.m[3]);
		//c.m[7]= (this->m[3] * b.m[4]) + (this->m[7] * b.m[5]) + (this->m[11] * b.m[6]) + (this->m[15] * b.m[7]);
		//c.m[11]= (this->m[3] * b.m[8]) + (this->m[7] * b.m[9]) + (this->m[11] * b.m[10]) + (this->m[15] * b.m[11]);
		//c.m[15]= (this->m[3] * b.m[12]) + (this->m[7] * b.m[13]) + (this->m[11] * b.m[14]) + (this->m[15] * b.m[15]);

		return c;
	};

	void* operator new(size_t s)
	{
		void *a = _aligned_malloc(s, 16);
		//printf("matrix4x4f::new %d\n", IsAlign(a));
		return a;
	};

	void operator delete(void *a)
	{
		_aligned_free(a);
	};

	void mmul(const float * a, const float * b)//, float * r)
	{
		matrix4x4f c;
		c.mmul_sse(a, b); //, float * r)
		*this = c;
	};

	void mmul_sse(const float * a, const float * b)//, float * r)
	{
		__m128 a_line, b_line, r_line;
		for (int i=0; i<16; i+=4) {
			// unroll the first step of the loop to avoid having to initialize r_line to zero
			a_line = _mm_load_ps(a);         // a_line = vec4(column(a, 0))
			b_line = _mm_set1_ps(b[i]);      // b_line = vec4(b[i][0])
			r_line = _mm_mul_ps(a_line, b_line); // r_line = a_line * b_line
			for (int j=1; j<4; j++) {
				a_line = _mm_load_ps(&a[j*4]); // a_line = vec4(column(a, j))
				b_line = _mm_set1_ps(b[i+j]);  // b_line = vec4(b[i][j])
				// r_line += a_line * b_line
				r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);
			}
			_mm_store_ps(&m[i], r_line);     // r[i] = r_line
		}
	};

	void mmul_sse_unroll(const float * a, const float * b)//, float * r)
	{
		__m128 s0, x0, x1, x2, x3, r_line;
		
		// unroll the first step of the loop to avoid having to initialize r_line to zero

		// carrego apenas uma vez
		x0 = _mm_load_ps(a);         // a_line = vec4(column(a, 0))
		x1 = _mm_load_ps(&a[4]); // a_line = vec4(column(a, j))
		x2 = _mm_load_ps(&a[8]); // a_line = vec4(column(a, j))
		x3 = _mm_load_ps(&a[12]); // a_line = vec4(column(a, j))

		for (int i=0; i<16; i+=4)
		{
			s0 = _mm_set1_ps(b[i]);      // b_line = vec4(b[i][0])
			r_line = _mm_mul_ps(x0, s0); // r_line = a_line * b_line
			
			s0 = _mm_set1_ps(b[i+1]);  // b_line = vec4(b[i][j])
			r_line = _mm_add_ps(_mm_mul_ps(x1, s0), r_line);
			
			s0 = _mm_set1_ps(b[i+2]);  // b_line = vec4(b[i][j])
			r_line = _mm_add_ps(_mm_mul_ps(x2, s0), r_line);
			
			s0 = _mm_set1_ps(b[i+3]);  // b_line = vec4(b[i][j])
			r_line = _mm_add_ps(_mm_mul_ps(x3, s0), r_line);
			
			_mm_store_ps(&m[i], r_line);     // r[i] = r_line
		}
	};

	vector4f getRow(unsigned char n)
	{
		vector4f a;

		if(n > 3) n=3;
		a.x = m[n+0];
		a.y = m[n+4];
		a.z = m[n+8];
		a.w = m[n+12];
		return a;
	};

	vec3f getRow3f(unsigned char n)
	{
		vec3f a;

		if(n > 3) n=3;
		a.x = m[n+0];
		a.y = m[n+4];
		a.z = m[n+8];
		// a.w = m[n+12];
		return a;
	};

	void setRow(unsigned char n, vec3f v)
	{
		if(n>3)
			return;
		//n=n*4;
		m[n] = v.x;
		m[n+4] = v.y;
		m[n+8] = v.z;
		return;
	};


	void setColumn(unsigned char n, vec3f v)
	{
		if(n>3)
			return;
		n=n*4;
		m[n+0] = v.x;
		m[n+1] = v.y;
		m[n+2] = v.z;
		return;
	};

	void setColumn(unsigned char n, vector4f v)
	{
		if(n>3)
			return;
		n=n*4;
		m[n+0] = v.x;
		m[n+1] = v.y;
		m[n+2] = v.z;
		m[n+3] = v.w;
		return;
	};

	void translate(vec3f a)
	{
		m[12] += a.x;
		m[13] += a.y;
		m[14] += a.z;
	};

	void scale(vec3f a)
	{
		m[0] *= a.x;
		m[4] *= a.x;
		m[8] *= a.x;
		m[12] *= a.x;

		m[1] *= a.y;
		m[5] *= a.y;
		m[9] *= a.y;
		m[13] *= a.y;

		m[2] *= a.z;
		m[6] *= a.z;
		m[10] *= a.z;
		m[14] *= a.z;
	};
	
    void rotate(const float &angle, vec3f &axis);
    //void transformVector( vec3f &vec );
	void transformVector( vec3f *vec );

	void transformVectorSet(vec3f *vec, int n);
	void print();
	void transpose3x3()
	{
		for(int i=0; i < 3; i++)
			for(int j=i+1; j < 3; j++)
			//{
				::swap(m[ijn(j,i,4)], m[ijn(i,j,4)]);
			//}
	};
	
	void Q2M(float q[4])
	{
		//q[0]=0;
		gotoxys(0,30); printf("QUAT %f %f %f %f len(%f)\n",
			q[0],q[1],q[2],q[3],
			sqrt(pow(q[0],2.0f)+pow(q[1],2.0f)+pow(q[2],2.0f)+pow(q[3],2.0f)));
		
		/*vec4 *s;
		vec4 S;
		s = (vec4*) q;
		s->norm();
		S = *s;*/
		
		//s->x = S.w;
		//s->y = S.x;
		//s->z = S.y;
		//s->w = S.z;

		//q[0] = q[0] * sin(q[3]/2.0);// * piover180);
		//q[1] = q[1] * sin(q[3]/2.0);//* piover180);
		//q[2] = q[2] * sin(q[3]/2.0);//* piover180);
		//q[3] = cos(q[3]/2.0);

    double sqw = q[3]*q[3];
    double sqx = q[0]*q[0];
    double sqy = q[1]*q[1];
    double sqz = q[2]*q[2];

    // invs (inverse square length) is only required if quaternion is not already normalised
    double invs = 1 / (sqx + sqy + sqz + sqw);
    m[0] = ( sqx - sqy - sqz + sqw)*invs ; // since sqw + sqx + sqy + sqz =1/invs*invs
    m[5] = (-sqx + sqy - sqz + sqw)*invs ;
    m[10] = (-sqx - sqy + sqz + sqw)*invs ;
    
    double tmp1 = q[0]*q[1];
    double tmp2 = q[2]*q[3];
    m[1] = 2.0 * (tmp1 + tmp2)*invs ;
    m[4] = 2.0 * (tmp1 - tmp2)*invs ;
    tmp1 = q[0]*q[2];
    tmp2 = q[1]*q[3];
    m[2] = 2.0 * (tmp1 - tmp2)*invs ;
    m[8] = 2.0 * (tmp1 + tmp2)*invs ;
    tmp1 = q[1]*q[2];
    tmp2 = q[0]*q[3];
    m[6] = 2.0 * (tmp1 + tmp2)*invs ;
    m[9] = 2.0 * (tmp1 - tmp2)*invs ;   

		//m[0] = q[3]*q[3] + q[0]*q[0] - q[1]*q[1] - q[2]*q[2];
		//m[1] = 2*(q[0]*q[1]+q[2]*q[3]);
		//m[2] = 2*(q[0]*q[2]-q[1]*q[3]);

		//m[4] = 2*(q[0]*q[1]-q[2]*q[3]);
		//m[5] = q[3]*q[3] - q[0]*q[0] + q[1]*q[1] - q[2]*q[2];
		//m[6] = 2*(q[1]*q[2]+q[0]*q[3]);

		//m[8] = 2*(q[0]*q[2]+q[1]*q[3]);
		//m[9] = 2*(q[1]*q[2]-q[0]*q[3]);
		//m[10] = q[3]*q[3] - q[0]*q[0] - q[1]*q[1] + q[2]*q[2];

		//m[0] = q[0]*q[0] + q[3]*q[3] - q[2]*q[2] - q[1]*q[1];
		//m[1] = 2*(q[3]*q[2]+q[1]*q[0]);
		//m[2] = 2*(q[3]*q[1]-q[2]*q[0]);

		//m[4] = 2*(q[3]*q[2]-q[1]*q[0]);
		//m[5] = q[0]*q[0] - q[3]*q[3] + q[2]*q[2] - q[1]*q[1];
		//m[6] = 2*(q[2]*q[1]+q[3]*q[0]);

		//m[8] = 2*(q[3]*q[1]+q[2]*q[0]);
		//m[9] = 2*(q[2]*q[1]-q[3]*q[0]);
		//m[10] = q[0]*q[0] - q[3]*q[3] - q[2]*q[2] + q[1]*q[1];
	};

	void swap(int u, int v)
	{
		vec3f a = getRow3f(u);
		setRow(u, getRow3f(v));
		setRow(v, a);
	};
};

// Operações sobre matrizes
void transpose3x3(double *ori, double *des);
void mulMatrix(double *a, double *b, double *c, int m, int n, int p);
void calcInverse(double *inverse, double *r);

#endif // _MATRIX4X4F_H_

