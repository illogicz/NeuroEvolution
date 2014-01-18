#pragma once
#include "../sPhysics/sContainer.h"
#include "sGenome.h"

// Virtual class

class sPhenotype : public sContainer
{
public:
	virtual float getFitness() = 0;
	sGenome genome;
	int lifeTime;
	bool alive;
};