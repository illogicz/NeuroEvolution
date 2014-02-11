#pragma once

float tanh_approx(float x)
{
	return tanhf(x);
	if( x < -3.f )
		return -1.f;
	else if( x > 3.f )
		return 1.f;
	else
		return x * ( 27.f + x * x ) / ( 27.f + 9.f * x * x );
}