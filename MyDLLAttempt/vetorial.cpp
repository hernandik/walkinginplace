#include "stdafx.h"
#include "vetorial.h"


vec3f vec3f::operator + ( void ) 
{
    return *this;
}

vec3f vec3f::operator - ( void ) 
{
    vec3f vResult(-x, -y, -z);
    return vResult;
}

vec3f& vec3f::operator += ( const vec3f &other )
{
	x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

vec3f& vec3f::operator -= ( const vec3f &other )
{
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

vec3f operator * ( const float scalar, const vec3f &other )
{
    vec3f vResult(0.0f, 0.0f, 0.0f);


    vResult.x = other.x * scalar;
    vResult.y = other.y * scalar;
    vResult.z = other.z * scalar;

    return vResult;
}

vec3f operator / ( const float scalar, const vec3f &other )
{
    vec3f vResult(0.0f, 0.0f, 0.0f);

    vResult.x = other.x / scalar;
    vResult.y = other.y / scalar;
    vResult.z = other.z / scalar;

    return vResult;
}

float dotProduct( const vec3f &v1,  const vec3f &v2 )
{
    return( (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z)  );
}

vec3f crossProduct( const vec3f &v1,  const vec3f &v2 )
{
    vec3f vCrossProduct;

    vCrossProduct.x =  (v1.y * v2.z) - (v1.z * v2.y);
    vCrossProduct.y = -(v1.x * v2.z) + (v1.z * v2.x);
    vCrossProduct.z =  (v1.x * v2.y) - (v1.y * v2.x);

    return vCrossProduct;
}

// Funções legadas
// Angle utilizado apenas pelo codigo das balance boards, com uma correção em caso de problemas
float angle(vec3f *a, vec3f *b)
{
	float dot, c;
	
	dot = dotProduct(*a, *b);
	c = a->len() * b->len();
	if(c != c)
		c = 1;
	if(fabs(c) < 0.00001)
		dot = 0;
	else{
		dot = dot / c;
		dot = acosf(dot);
	}
	return dot;
}

// Operações massivas sobre vertices
void scaleV(vec3f *a, float b, int n)
{
	for (int i=0; i<n; i++){
		*a = *a * b;
		a++;
	}
}

void scaleV(vec3f *in, vec3f b, int n)
{
	for (int i=0; i<n; i++){
		*in = *in * b;
		in++;
	}
}

void translate(vec3f *vertices, vec3f posicao, int num)
{
	int i;
	for(i = 0; i < num; i++)
	{
		vertices[i] = vertices[i] + posicao;
	}
	return;
}

void mulVertex(vec3f *a, vec3f b, int n)
{
	for (int i=0; i < n; i++)
	{
		*a = *a * b;
		a++;
	}
}

vector4f get4f(vec3f &a){

	vector4f b;
	b.x = a.x;
	b.y = a.y;
	b.z = a.z;
	return b;
}

vector4f vector4f::operator= (const vector4f &a){
	x = a.x;
	y = a.y;
	z = a.z;
	w = a.w;
	return *this;
}

vector4f vector4f::operator= (const vec3f &a){
	x = a.x;
	y = a.y;
	z = a.z;
	w = 1.0; // assure that this man is 1
	return *this;
}

vector4f vector4f::operator+ (const float &a){
	vector4f b;
	b.x = this->x;
	b.y = this->y;
	b.z = this->z;

	b.w = a;
	return b;
}

