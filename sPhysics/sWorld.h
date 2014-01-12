#pragma once
#include <Box2D\Box2D.h>
#include <set>
#include <vector>
#include "sBody.h"
#include "sContainer.h"

using std::set;
using std::vector;
class sBody;

class sContactListener
{
public:
	virtual void onBeginContact(){}
	virtual void onEndContact(){}
};

class sStepListener
{
public:
	virtual void onBeforeStep(){}
	virtual void onAfterStep(){}
};

class sWorld : public sContainer
{

public:


	sWorld():gravity(0.0f, -9.8f), b2world(gravity)
	{
		m_world = this; //&b2world;
		//b2world.SetDestructionListener(&m_destructionListener);

		//b2world.SetContactListener();
		timeStep = 1.f/60.f;
		velocityIterations = 8;
		positionIterations = 3;
		b2world.SetGravity(b2Vec2(0.0f, -9.8f));
		// Set to true, because this is the world itself
		m_inWorld = true;
	}


	void step()
	{
		//dispatchStepEvent(true);
		b2world.Step(timeStep, velocityIterations, positionIterations);
		//dispatchStepEvent(true);
	}


	b2World b2world;
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
		b2world.QueryAABB(&cb, aabb);
		return cb.bodies;
	}
	



	
	// Manage listener for physics step events
	void addStepListener(sStepListener &listener)
	{
		if(m_stepListeners.find(&listener) != m_stepListeners.end()){
			printf("Warning: sWorld::addStepListener(), listener already added\n");
		} else {
			m_stepListeners.insert(&listener);
		}
	}
	void removeStepListener(sStepListener &listener)
	{
		if(m_stepListeners.find(&listener) != m_stepListeners.end()){
			m_stepListeners.erase(&listener);
		} else {
			printf("Warning: sWorld::removeStepListener(), listener not found\n");
		}
	}


	// Manage listener for contact events
	// If the last argument is omitted, contact with any other body will fire
	void addContactListsner(sContactListener &listener, sBody *body1, sBody *body2 = NULL)
	{

	}
	void removeContactListener(sContactListener &listener, sBody *body1, sBody *body2 = NULL)
	{

	}

	// TODO: Bad, exposes box2d interface.
	void addContactListsner(sContactListener &listener, b2FixtureDef *body1, b2FixtureDef *body2 = NULL)
	{

	}
	void removeContactListener(sContactListener &listener, b2FixtureDef *body1, b2FixtureDef *b2FixtureDef = NULL)
	{

	}
	
	 //temp public
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
	
	//class GlobalContactListener : public b2ContactListener
	//{
	//public:
		
	//}


	// map<sBody*, map<sBody*, set<sContactListener*>>> m_contactListeners;


	
	
	set<sStepListener*> m_stepListeners;
	void dispatchStepEvent(bool before = true)
	{
		set<sStepListener*>::iterator i;
		for(i = m_stepListeners.begin(); i != m_stepListeners.end(); ++i){
			if(before){
				(*i)->onBeforeStep();
			} else {
				(*i)->onAfterStep();
			}
		}
	}
	


};




