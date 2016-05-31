#include "stdafx.h"
#include "matrix4x4f.h"

//#include "glext.h"

matrix4x4f::matrix4x4f( float m0, float m4, float  m8, float m12,
                        float m1, float m5, float  m9, float m13,
                        float m2, float m6, float m10, float m14,
                        float m3, float m7, float m11, float m15 )
{
    m[0]=m0; m[4]=m4; m[8] =m8;  m[12]=m12;
    m[1]=m1; m[5]=m5; m[9] =m9;  m[13]=m13;
    m[2]=m2; m[6]=m6; m[10]=m10; m[14]=m14;
    m[3]=m3; m[7]=m7; m[11]=m11; m[15]=m15;
}

void matrix4x4f::identity( void )
{
	for(int i=0; i<15; i++)
		m[i] = 0;

	
	//memset(m,0,sizeof(float)*16);
	m[15]=m[10]=m[5]=m[0]=1.0f;
}

void matrix4x4f::identitySlow( void )
{
	memset(m,0,sizeof(float)*16);
	m[15]=m[10]=m[5]=m[0]=1.0f;
}

void matrix4x4f::identityFast( void )
{
	__m128 a;//,b;//,b,c,d;
	a = _mm_setzero_ps();
	//b = _mm_set_ss(1.0f);
	//b = _mm_shuffle_ps(a,a,_MM_SHUFFLE(1,1,0,1));
	//c = _mm_shuffle_ps(b,b,_MM_SHUFFLE(0,1,0,0));
	//d = _mm_shuffle_ps(c,c,_MM_SHUFFLE(2,0,0,0));
	//_mm_store_ps(&m[0],a);
	//_mm_store_ps(&m[4],b);
	//_mm_store_ps(&m[8],c);
	//_mm_store_ps(&m[12],d);
	_mm_store_ps(&m[0],a);
	_mm_store_ps(&m[4],a);
	_mm_store_ps(&m[8],a);
	_mm_store_ps(&m[12],a);
	m[15]=m[10]=m[5]=m[0]=1.0f;
}

void matrix4x4f::set( float m0, float m4, float  m8, float m12,
                        float m1, float m5, float  m9, float m13,
                        float m2, float m6, float m10, float m14,
                        float m3, float m7, float m11, float m15 )
{
    m[0]=m0; m[4]=m4; m[8] =m8;  m[12]=m12;
    m[1]=m1; m[5]=m5; m[9] =m9;  m[13]=m13;
    m[2]=m2; m[6]=m6; m[10]=m10; m[14]=m14;
    m[3]=m3; m[7]=m7; m[11]=m11; m[15]=m15;
}

void matrix4x4f::rotate( const float &angle, vec3f &axis )
{
    float s = sin(DEGTORAD(angle));
    float c = cos(DEGTORAD(angle));

    axis.norm();

    float ux = axis.x;
    float uy = axis.y;
    float uz = axis.z;
    
    m[0]  = c + (1-c) * ux*ux;
    m[1]  = (1-c) * ux*uy + s*uz;
    m[2]  = (1-c) * ux*uz - s*uy;
    m[3]  = 0;
    
    m[4]  = (1-c) * uy*ux - s*uz;
    m[5]  = c + (1-c) * pow(uy,2);
    m[6]  = (1-c) * uy*uz + s*ux;
    m[7]  = 0;
    
    m[8]  = (1-c) * uz*ux + s*uy;
    m[9]  = (1-c) * uz*uy - s*ux;
    m[10] = c + (1-c) * pow(uz,2);
    m[11] = 0;
    
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;
}

void matrix4x4f::transformVectorSet(vec3f *vec, int n)
{
	__declspec(align(16)) float mm[16];

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			mm[ijn(i,j,4)] = m[ijn(j,i,4)];

	__m128 a,b,c,d, I;
	
	a = _mm_load_ps(&mm[0]);
	b = _mm_load_ps(&mm[4]);
	c = _mm_load_ps(&mm[8]);
	d = _mm_load_ps(&mm[12]);

	for(int i=0; i < n; i++)
	{
		I = _mm_loadu_ps((float *)&vec[i]);

		__m128 m0 = _mm_mul_ps(a, I);
		__m128 m1 = _mm_mul_ps(b, I);
		__m128 m2 = _mm_mul_ps(c, I);
		__m128 m3 = _mm_mul_ps(d, I);
		__m128 sum_01 = _mm_hadd_ps(m0, m1);
		__m128 sum_23 = _mm_hadd_ps(m2, m3);
		__m128 result = _mm_hadd_ps(sum_01, sum_23);
		_mm_storeu_ps((float*)&vec[i], result);

		//_mm_storeu_ps((float*) &vec[i], O);
	}
	//transformVector(&vec[i]);
}


//void matrix4x4f::transformVectorSet(vec3f *vec, int n)
//{
//	for(int i=0; i < n; i++)
//		transformVector(&vec[i]);
//}

void matrix4x4f::transformVector( vec3f *v )
{
    //vec3f &v = *vec;

    float x = v->x;
    float y = v->y;
    float z = v->z;

    v->x = x * m[0] +
          y * m[4] +
          z * m[8] + m[12];

    v->y = x * m[1] +
          y * m[5] +
          z * m[9] + m[13];

    v->z = x * m[2] +
          y * m[6] +
          z * m[10] + m[14];
}

void matrix4x4f::transformVector( vector4f *v )
{
    //vec3f &v = *vec;

    float x = v->x;
    float y = v->y;
    float z = v->z;
	float w = v->w;

    v->x = x * m[0] +
          y * m[4] +
          z * m[8] + 
		  w * m[12];

    v->y = x * m[1] +
          y * m[5] +
          z * m[9] + 
		  w * m[13];

    v->z = x * m[2] +
          y * m[6] +
          z * m[10] +
		  w * m[14];
	
	v->w = x * m[3] +
          y * m[7] +
          z * m[11] +
		  w * m[15];
}
//void matrix4x4f::transformVector( vec3f &v )
//{
//    //vec3f &v = *vec;
//
//    float x = v.x;
//    float y = v.y;
//    float z = v.z;
//
//    v.x = x * m[0] +
//          y * m[4] +
//          z * m[8] + m[12];
//
//    v.y = x * m[1] +
//          y * m[5] +
//          z * m[9] + m[13];
//
//    v.z = x * m[2] +
//          y * m[6] +
//          z * m[10] + m[14];
//}

void matrix4x4f::print()
{
	for(int i=0; i<16; i++){
		printf("%3.2f ",m[i]);
		if((i%4) == (3)) printf("\n");
	}
}

void mulMatrix(double *a, double *b, double *c, int m, int n, int p)
{
	int i, j, k;
	
	for(i=0; i < m; i++){ // linha a
		for (j=0; j < p; j++){ // coluna b
			for(k=0; k < n; k++) // 
				c[j+i*m] += (a[k+i*m] * b[j+k*p]);
		}
	}
}

void transpose3x3(double *ori, double *des)
{
	for(int i=0; i < 3; i++)
		for(int j=0; j<3; j++)
			des[ij(j,i)] = ori[ij(i,j)];
}

//void updateViewMatrix( CAM c )
//{
//	matrix4x4f view;
//	//view.identity();
//
//	// normalize the lookat vector. We will call this vector "n" 
//	c.look.norm();
//    // compute and normalize the "X-direction" of the camera coordinate system.
//	// We will call this vector "u"
//	c.right = crossProduct(c.look, c.up);
//	c.right.norm();
//    // compute and normalize the "Y-direction" of the camera coordinate system.
//	// We will call this vector "v"
//	c.up = crossProduct(c.right, c.look);
//	c.up.norm();
//
//	// set the elements of the view matrix as a ModelView
//	view.m[0] =  c.right.x;
//	view.m[1] =  c.up.x;
//	view.m[2] = -c.look.x;
//	view.m[3] =  0.0f;
//
//	view.m[4] =  c.right.y;
//	view.m[5] =  c.up.y;
//	view.m[6] = -c.look.y;
//	view.m[7] =  0.0f;
//
//	view.m[8]  =  c.right.z;
//	view.m[9]  =  c.up.z;
//	view.m[10] = -c.look.z;
//	view.m[11] =  0.0f;
//
//	view.m[12] = -dotProduct(c.right, c.eye);
//	view.m[13] = -dotProduct(c.up, c.eye);
//	view.m[14] =  dotProduct(c.look, c.eye);
//	view.m[15] =  1.0f;
//
//	// multiply the current ModelView matrix (current an Identity Matrix) 
//	// by the content of the matrix view
//	glMultMatrixf( view.m );
//}
//
//void updateViewMatrix( CAM c, matrix4x4f *m)
//{
//	matrix4x4f view;
//	//view.identity();
//
//	// normalize the lookat vector. We will call this vector "n" 
//	c.look.norm();
//    // compute and normalize the "X-direction" of the camera coordinate system.
//	// We will call this vector "u"
//	c.right = crossProduct(c.look, c.up);
//	c.right.norm();
//    // compute and normalize the "Y-direction" of the camera coordinate system.
//	// We will call this vector "v"
//	c.up = crossProduct(c.right, c.look);
//	c.up.norm();
//
//	// set the elements of the view matrix as a ModelView
//	view.m[0] =  c.right.x;
//	view.m[1] =  c.up.x;
//	view.m[2] = -c.look.x;
//	view.m[3] =  0.0f;
//
//	view.m[4] =  c.right.y;
//	view.m[5] =  c.up.y;
//	view.m[6] = -c.look.y;
//	view.m[7] =  0.0f;
//
//	view.m[8]  =  c.right.z;
//	view.m[9]  =  c.up.z;
//	view.m[10] = -c.look.z;
//	view.m[11] =  0.0f;
//
//	view.m[12] = -dotProduct(c.right, c.eye);
//	view.m[13] = -dotProduct(c.up, c.eye);
//	view.m[14] =  dotProduct(c.look, c.eye);
//	view.m[15] =  1.0f;
//
//	// multiply the current ModelView matrix (current an Identity Matrix) 
//	// by the content of the matrix view
//	
//	*m = *m * view;
//	//glMultMatrixf( view.m );
//}
