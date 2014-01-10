#pragma once
#include <set>
#include "sContainer.h"

using std::set;

class sWorld : public sContainer
{

public:


	sWorld():gravity(0.0f, -9.8f), __world(gravity)
	{
		m_world = &__world;
		__world.SetDestructionListener(&m_destructionListener);
		timeStep = 1.f/60.f;
		velocityIterations = 8;
		positionIterations = 3;
		m_inWorld = true;
	}


	void step()
	{
		m_world->Step(timeStep, velocityIterations, positionIterations);
	}

	float32 timeStep;
	int32 velocityIterations;
	int32 positionIterations;
	b2Vec2 gravity;

	b2World __world; //temp public

protected:



private:

	sContainer::addToWorld; // disable these
	sContainer::removeFromWorld;

	class DestructionListener : public b2DestructionListener
	{
		void SayGoodbye(b2Joint * joint)
		{
			//sJoint * j = (sJoint*)joint->GetUserData();
			//j->jointDestroyed();
		}
		void SayGoodbye(b2Fixture * fixture){}
	};
	DestructionListener m_destructionListener;


};