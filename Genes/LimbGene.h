#pragma once

#include <vector>
#include "BaseGene.h"


class LimbGene : public BaseGene
{

public:

	LimbGene copy(LimbGene &g)
	{
		BaseGene::copy(BaseGene(g));
		g.density = copyValue(density, 0.1f,2.f);
		for(int i = 0; i < shape.size(); i++){
			g.shape.push_back(shape[i]);
		}
	}

protected:

	int density;
	std::vector<b2Vec2> shape;
	
	b2Vec2 anchor;
	std::vector<b2Vec2> child_anchors;
};