#pragma once

#include <random>
#include <Box2D\Common\b2Math.h>

class BaseGene
{
	
public:
	
	void copy(BaseGene &target)
	{
		target.mutation_rate = copyFloat(mutation_rate, 0, 0.1);
	}

	float copyFloat(float v, float min, float max)
	{
		float r = (2.f * rand() / RAND_MAX) - 1.f;
		r *= r * (r < 0 ? -1 : 1) * mutation_rate;
		v += r;
		if(v > max) v = max;
		if(v < min) v = min;
		return v;
	}

protected:

	float mutation_rate;
	
	//float 




};