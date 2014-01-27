#include "../../sPhysics/sConvexPolygon.h"

class WormSegment : public sConvexPolygon
{

public:
	WormSegment(){}
	int index;


	void applyLateralDamping(float t)
	{
		if(t == 0)return;
		b2Vec2 v = b2Mul(b2Rot(-getAngle()), getLinearVelocity());

		v.y *= 1.f - t;

		setLinearVelocity(b2Mul(b2Rot(getAngle()), v));
		
	}


};