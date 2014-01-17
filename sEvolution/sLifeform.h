#pragma once
#include "sGenome.h"

// Virtual class

class sLifeForm
{
public:
	virtual float getFitness()
	{
		return 0;	
	};
	sGenome genome;
	int lifeTime;
};