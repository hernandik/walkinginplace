#ifndef _QUAT
#define _QUAT

#include "vetorial.h"
//#include "basic.h"

class Quat : public vector4f
{

public:
	using vector4f::operator=;
	//Quat operator= (const vector4f &a)
	//{
	//	x = a.x;
	//	y = a.y;
	//	z = a.z;
	//	w = a.w;

	//	return *this;
	//};

	Quat operator* (const Quat &a){

		Quat s;
		//s.A = a.A*b.A - a.B*b.B - a.C*b.C - a.D*b.D;
		//s.B = a.A*b.B + a.B*a.A + a.C*b.D - a.D*b.C;
		//s.C = a.A*b.C - a.B*b.D + a.C*b.A + a.D*b.B;
		//s.D = a.A*b.D + a.B*b.C - a.C*b.B + a.D*b.A;
		s.A = A*a.A - B*a.B - C*a.C - D*a.D;
		s.B = A*a.B + B*a.A + C*a.D - D*a.C;
		s.C = A*a.C - B*a.D + C*a.A + D*a.B;
		s.D = A*a.D + B*a.C - C*a.B + D*a.A;
		return s;
	};

	Quat conj()//const vector4f &in
	{
		Quat b;
		b.set(A,-B,-C,-D);
		return b;
	};

	Quat inverse()//const vector4f &in
	{
		Quat b;
		b = conj();
		float n;
		n = len();
		b.set(b.A/n, b.B/n, b.C/n, b.D/n);
		return b;
	};
	
	// gen quaternion based on this values
	void ypr(float *ypr)
	{
		float angle;

		angle = ypr[0] * 0.5f;
		float sr = sin(angle);
		float cr = cos(angle);

		angle = ypr[1] * 0.5f;
		float sp = sin(angle);
		float cp = cos(angle);

		angle = ypr[2] * 0.5f;
		float sy = sin(angle);
		float cy = cos(angle);

		float cpcy = cp * cy;
		float spcy = sp * cy;
		float cpsy = cp * sy;
		float spsy = sp * sy;

		x = (sr * cpcy - cr * spsy);
		y = (cr * spcy + sr * cpsy);
		z = (cr * cpsy - sr * spcy);
		w = (cr * cpcy + sr * spsy);

		norm();
	};
};

Quat QuatSlerp( Quat u, Quat v, float f );
//static void matrix2YPR(float m[3][3], float ypr[3]);
//static void q2M(float q[4], float m[3][3]);


static void q2M(float q[4], float m[3][3])
{
	m[0][0] = q[3]*q[3] + q[0]*q[0] - q[1]*q[1] - q[2]*q[2];
	m[0][1] = 2*(q[0]*q[1]+q[2]*q[3]);
	m[0][2] = 2*(q[0]*q[2]-q[1]*q[3]);

	m[1][0] = 2*(q[0]*q[1]-q[2]*q[3]);
	m[1][1] = q[3]*q[3] - q[0]*q[0] + q[1]*q[1] - q[2]*q[2];
	m[1][2] = 2*(q[1]*q[2]+q[0]*q[3]);

	m[2][0] = 2*(q[0]*q[2]+q[1]*q[3]);
	m[2][1] = 2*(q[1]*q[2]-q[0]*q[3]);
	m[2][2] = q[3]*q[3] - q[0]*q[0] - q[1]*q[1] + q[2]*q[2];
};



static void matrix2YPR(float m[3][3], float ypr[3])
{
	ypr[0] = atan2(m[0][1], m[0][0]);
	ypr[1] = asin(m[0][2]);
	ypr[2] = -atan2(m[1][2], m[2][2]);
};


#endif