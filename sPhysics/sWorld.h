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
		//__world.SetContactListener();

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

	

	// Gets bodies under a specified point
	vector<b2Body*> getBodiesAt(b2Vec2 position)
	{
		b2AABB aabb;
		aabb.lowerBound.Set(position.x, position.y);
		aabb.upperBound.Set(position.x, position.y);

		PointQueryCallback cb(position);
		m_world->QueryAABB(&cb, aabb);
		return cb.bodies;
	}


	b2World __world; //temp public
protected:



private:

	sContainer::addToWorld;       // disable
	sContainer::removeFromWorld;  // disable



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





	class PointQueryCallback : public b2QueryCallback
	{
	public:
		PointQueryCallback(b2Vec2 position){
			m_position = position;
		}
		bool ReportFixture(b2Fixture* fixture){
			if(fixture->TestPoint(m_position)){
				bodies.push_back(fixture->GetBody());
				return false;
			}
			return true;
		}
		vector<b2Body*> bodies;
		b2Vec2 m_position;
	};

	//class ContactListener : public b2ContactListener
	//{
	//public:
		
	//}


};



class sContactListener
{
public:
	virtual void onBeginContact(){}
	virtual void onEndContact(){}
};

class sStepistener
{
public:
	virtual void onBeforeStep(){}
	virtual void onAfterStep(){}
};
