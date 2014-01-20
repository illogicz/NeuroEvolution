#pragma once
#include "sSynapse.h"

class sNeuron
{

public:


	void addInput(float w)
	{
		
	}

	float getOutput()
	{
		return m_output;
	}


private:

	float m_output;
	float m_input;

	float tanh_approx(float x)
	{
		x *= 3.f;
		if( x < -3.f )
			return -1.f;
		else if( x > 3.f )
			return 1.f;
		else
			return x * ( 27.f + x * x ) / ( 27.f + 9.f * x * x );
	}


};