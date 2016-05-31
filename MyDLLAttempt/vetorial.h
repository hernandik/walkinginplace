#ifndef _VETOR
#define _VETOR

#define XY 0
#define XZ 1
#define YZ 2

#include <math.h>
#include <stdio.h>
#include <fvec.h>
#include <mmintrin.h>
#include <emmintrin.h>

// #include "color.h"
//#include <GL/glut.h>
class vec3f;

//POINT2D
class vec2f{
public:
	union {
		struct {
			float u,v;
		};
		struct {
			float x,y;
		};
	};
};

typedef struct tagRGBByte{
	unsigned char r,g,b;
} RGBbyte;


class RGBAbyte {
public:

	union{
		struct{
			unsigned char r,g,b,a;
		};
		unsigned int i;// formato inteiro
	};

	RGBAbyte()
	{
		set(255,255,255,255);// branco sempre
	};

	void set(UCHAR _r,UCHAR _g, UCHAR _b,UCHAR _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	};
};// RGBAbyte;

class vector4f
{
public:
	union{
		struct{
			float x,y,z,w;	// vetor
		};
		struct{
			float r,g,b,a; // RGB
		};
		struct {
			float h,s,v; // HSV
		};
		struct{
			float A,B,C,D;
		};
	};
	
	vector4f()
	{
		x=y=z=w=0;
	};
	
	friend vector4f mix(vector4f a, vector4f b, float s);

	vector4f(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	};

	vector4f operator* (const vector4f &a){
		vector4f b;
		b = *this;
		b.x *= a.x;
		b.y *= a.y;
		b.z *= a.z;
		b.w *= a.w;
		return b;
	};

	//vector4f operator= (const vec3f &a){
	//	this->x = a.x;
	//	this->y = a.y;
	//	this->z = a.z;
	//	w = a.w; // copia
	//	return *this;
	//};

	vector4f(double _x, double _y, double _z, double _w)
	{
		x = (float) _x;
		y = (float) _y;
		z = (float) _z;
		w = (float) _w;
	};

	vector4f(int _x, int _y, int _z, int _w)
	{
		x = (float) _x;
		y = (float) _y;
		z = (float) _z;
		w = (float) _w;
	};

	int getColorInt()
	{
		//int i;
		RGBAbyte c;
		c.r = r * 255;
		c.g = g * 255;
		c.b = b * 255;
		c.a = a * 255;
		return c.i;
	};

	vector4f operator= (const vector4f &a);
	vector4f operator= (const vec3f &a);
	vector4f operator+ (const float &b);
	
	void addsse( vector4f *a, vector4f *b)
	{
		F32vec4 *av=(F32vec4 *) a;
		F32vec4 *bv=(F32vec4 *) b;
		F32vec4 *cv=(F32vec4 *) this;
		//F32vec4 av= _mm_load_ps((float*)a);
		//F32vec4 bv= _mm_load_ps((float*)b);
		//F32vec4 cv= _mm_load_ps((float*)this);

		*cv=*av + *bv;

		//_mm_store_ps((float*)this, cv);
		//this = (vector4f *) (cv);
	}
	
	void addsse(vector4f *a)
	{
		//F32vec4 *av=(F32vec4 *) a;
		//F32vec4 *bv=(F32vec4 *) b;
		//F32vec4 *cv=(F32vec4 *) this;
		F32vec4 av= _mm_load_ps((float*)a);
		//F32vec4 bv= _mm_load_ps((float*)b);
		F32vec4 cv= _mm_load_ps((float*)this);

		cv = av + cv;

		_mm_store_ps((float*)this, cv);
		//*a = (vector4f ) (*cv);
	}

	vector4f operator+ (const vector4f &a){
		vector4f b;
		b = *this;
		b.x = this->x + a.x;
		b.y = this->y + a.y;
		b.z = this->z + a.z;
		b.w = this->w + a.w;
		return b;
	};

	void set(float _x, float _y, float _z, float _w){
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	};

	vector4f operator* (const double &a){
		vector4f b;
		float c = (float) a;

		b = *this;
		b.x *= c;
		b.y *= c;
		b.z *= c;
		b.w *= c;
		return b;
	};

	void norm(){
		double a;
		a = 1.0/len();
		x = x * (float) a;
		y = y * (float) a;
		z = z * (float) a;
		w = w * (float) a;
	};

	double len(){
		return sqrt((x*x)+(y*y)+(z*z)+(w*w));
	};

	void print()
	{
		printf("x:%f y:%f z:%f w:%f\n", x,y,z,w);
	};
};

typedef __declspec( align( 16 ) ) vector4f vec4;

//typedef vector4f COLOR;
class vec3
{
public:
	union {
		//F32vec4 m;
		struct {
			float x, y, z;
		};
	};

	vec3()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    };

	vec3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	};

	void set(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	};

	void addsse (const vec3 *a, const vec3 *c)
	{
		//vec4 bb,aa;
		//aa = a;
		//bb = *this;
		//__declspec( align( 32 ) ) vec3f c;
		// __declspec( align( 16 )) vec3f c;

		//printf(" CC %d",  ((unsigned long)(&c) & 15));

		F32vec4 *av=(F32vec4 *) a;
		F32vec4 *bv=(F32vec4 *) this;
		F32vec4 *cv=(F32vec4 *) c;
		
		//for(int i=0; i<100000; i++)
		//{
			*cv = _mm_add_ps(*av, *bv);
			//*this = *c;
		//}

		//_mm_load_ps();
		//c->m = _mm_add_ps(a->m, b->m);

		//F32vec4 cc;
		//cc = _mm_add_ps((F32vec4) a->x, (F32vec4) this->x);
		//_mm_store_ps(&c.x, cc);
		
		//F32vec4 av= _mm_load_ps((float*) a);
		//F32vec4 bv= _mm_load_ps((float*) this);
		//F32vec4 cv= _mm_load_ps((float*) &c);

		//return c;
	};
	
	void print()
	{
		printf("%f %f %f\n", x,y,z);
	};
};

class vec3f
{
public:
	//double x, y, z;
	union {
		struct {
			float x, y, z, w;
		};
		//__m128 m;
	};

	//vec3f operator * (const float scalar);
    friend vec3f operator * (const float scalar, const vec3f &other);
	friend vec3f operator / ( const float scalar, const vec3f &other);

    vec3f& operator += (const vec3f &other);
    vec3f& operator -= (const vec3f &other);

    vec3f operator + (void);
    vec3f operator - (void);

    // Static utility methods
    static float distance(const vec3f &v1, const vec3f &v2);
    static float dotProduct(const vec3f &v1,  const vec3f &v2 );
    static vec3f crossProduct(const vec3f &v1, const vec3f &v2);

	vec3f operator+ (const vec3f &a){
		vec3f b;
		b = *this;
		
		b.x = this->x + a.x;
		b.y = this->y + a.y;
		b.z = this->z + a.z;
		b.w = this->z + a.w;

		return b;
	};

	void addsse (const vec3f *a, const vec3f *c)
	{
		//vec4 bb,aa;
		//aa = a;
		//bb = *this;
		//__declspec( align( 32 ) ) vec3f c;
		// __declspec( align( 16 )) vec3f c;

		//printf(" CC %d",  ((unsigned long)(&c) & 15));

		F32vec4 *av=(F32vec4 *) a;
		F32vec4 *bv=(F32vec4 *) this;
		F32vec4 *cv=(F32vec4 *) c;
		*cv = *av + *bv;
		
		//_mm_add_ps(*av, *bv);
		
		//F32vec4 cc;
		//cc = _mm_add_ps((F32vec4) a->x, (F32vec4) this->x);
		//_mm_store_ps(&c.x, cc);
		
		//F32vec4 av= _mm_load_ps((float*) a);
		//F32vec4 bv= _mm_load_ps((float*) this);
		//F32vec4 cv= _mm_load_ps((float*) &c);

		//return c;
	};

	vec3f operator- (const vec3f &a){
		vec3f b;
		b = *this;
		b.x = this->x - a.x;
		b.y = this->y - a.y;
		b.z = this->z - a.z;

		return b;
	};
	
	vec3f operator- (const vector4f &a){
		vec3f b;
		b = *this;
		b.x = this->x - a.x;
		b.y = this->y - a.y;
		b.z = this->z - a.z;

		return b;
	};

	vec3f operator= (const vec2f &a){
		this->x = a.x;
		this->y = a.y;
		//this->z = a.z;
		return *this;
	};

	vec3f operator= (const vector4f &a){
		this->x = a.x;
		this->y = a.y;
		this->z = a.z;
		return *this;
	};

	vec3f operator= (const vec3f &a){
		//struct tagvec3f b;
		//b = *this;
		x = a.x;
		y = a.y;
		z = a.z;
		return *this;
	};
	
	vec3f operator= (const float &a){
		//struct tagvec3f b;
		//b = *this;
		x = a;
		y = *((&a)+1);
		z = *((&a)+2);
		return *this;
	};

	vec3f operator= (const vec3 &a){
		//struct tagvec3f b;
		//b = *this;
		x = a.x;
		y = a.y;
		z = a.z;
		return *this;
	};
	//vec3f operator= (const struct {float x,y,z;} &a){
	//	//struct tagvec3f b;
	//	//b = *this;
	//	x = a.x;
	//	y = a.y;
	//	z = a.z;
	//	return *this;
	//};

	vec3f operator* (const double &a){
		vec3f b;
		b = *this;
		b.x *= float(a);
		b.y *= float(a);
		b.z *= float(a);
		return b;
	};

	vec3f operator/ (const double &a){
		vec3f b;
		b = *this;
		b.x /= float(a);
		b.y /= float(a);
		b.z /= float(a);
		return b;
	};

	vec3f operator/ (const float &a){
		vec3f b;
		b = *this;
		b.x /= float(a);
		b.y /= float(a);
		b.z /= float(a);
		return b;
	};

	vec3f operator* (const int &a){
		vec3f b;
		b = *this;
		b.x *= a;
		b.y *= a;
		b.z *= a;
		return b;
	};

	vec3f operator* (const float a){
		vec3f b;
		b = *this;
		b.x *= a;
		b.y *= a;
		b.z *= a;
		return b;
	};

	vec3f operator* (const vec3f &a){
		vec3f b;
		b = *this;
		b.x *= a.x;
		b.y *= a.y;
		b.z *= a.z;
		return b;
	};

	void print()
	{
		printf("%f %f %f\n", x,y,z);
	};

	double len(){
		return sqrt((this->x*this->x)+(this->y*this->y)+(this->z*this->z));
	};

	void norm(){
		double d = 1.0f / sqrt(x*x + y*y + z*z);
		if (d == 0.0) {
			//error("zero length vector");
			return;
		}
		x = x * float(d);
		y = y * float(d);
		z = z * float(d);
	};

	vec3f(void)
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
		w = 1.0f;
    };

	vec3f(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
		w = 1.0f;
	};

	vec3f abs()
	{
		vec3f a;
		a.x = fabs(x);
		a.y = fabs(y);
		a.z = fabs(z);
		return a;
	};

	void set(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
		w = 1.0;
	};
};

//typedef vec3f vec3f;
typedef vector4f COLOR;

float dotProduct( const vec3f &v1,  const vec3f &v2 );
float distance(const vec3f &v1, const vec3f &v2);
vec3f crossProduct(const vec3f &v1, const vec3f &v2);

float angle(vec3f *a, vec3f *b);

void scaleV(vec3f *a, float b, int n);

void translate(vec3f *vertices, vec3f posicao, int num);
void mulVertex(vec3f *a, vec3f b, int n);

float clamp(float f, float min, float max);

//float pertence(vec3f *a, vec3f *p);
//inline float dot3d(vec3f *a, vec3f *b);
//void normalize(vec3f *v);
//void normcrossprod(vec3f v1, vec3f v2, vec3f *saida);


#endif

