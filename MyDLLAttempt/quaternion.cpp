#include "stdafx.h"
#include "quaternion.h"

Quat QuatSlerp( Quat u, Quat v, float f )
{
	float alpha,beta,theta,sin_t,cos_t;
	int flip;
	Quat result;
 
    flip = 0;
 
	// Force the input within range.
	f = clamp(f,0.0f,1.0f);
	//f = fmin( f, 1.0f );
	//f = fmax( f, 0.0f );

	cos_t = u.x*v.x + u.y*v.y + u.z*v.z + u.w*v.w;
 
	if ( cos_t < 0.0f )
	{
		cos_t = -cos_t; flip=1;
	}
 
	if ( ( 1.0f - cos_t ) < 0.000001f )
	{
		beta 	= 1.0f - f;
		alpha 	= f;
 
	} else {
		theta 	= acos( cos_t );
		sin_t 	= sin( theta );
		beta 	= sin( theta - f * theta ) / sin_t;
		alpha 	= sin( f * theta ) / sin_t;
	}
 
	if ( flip != 0 ) alpha = -alpha;
 
	result.x = beta * u.x + alpha * v.x;
	result.y = beta * u.y + alpha * v.y;
	result.z = beta * u.z + alpha * v.z;
	result.w = beta * u.w + alpha * v.w;
 
	return result;
}



