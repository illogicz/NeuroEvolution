#pragma once
#include <Box2D\Box2D.h>
#include <set>
#include <map>
#include <vector>
#include "sBody.h"
#include "sContainer.h"
#include "sJoint.h"

using std::set;
using std::vector;
using std::multimap;
using std::map;
using std::make_pair;

class sBody;
class sJoint;



struct sContactPair
{ 
	sBody *body1; 
	sBody *body2; 

	bool contains(sBody *body)
	{
		return body == body1 || body == body2;
	}
	bool contains(sBody *bodyA, sBody *bodyB)
	{
		return contains(bodyA) && contains(bodyB);
	}

};
struct sContactPairCompare
{
	bool operator()(const sContactPair &left, const sContactPair &right) const
	{
		if(left.body1 != right.body1) return left.body1 < right.body1;
		return left.body2 < right.body2;
	}
};

class sContactListener
{
public:
	virtual void onBeginContact(sContactPair contactPair){}
	virtual void onEndContact(sContactPair contactPair){}
};

class sStepListener
{
public:
	virtual void onBeforeStep(){}
	virtual void onAfterStep(){}
};



class sWorld : public sContainer, public b2DestructionListener, public b2ContactListener
{

public:


	sWorld():gravity(0.0f, 9.8f), b2world(gravity)
	{
		m_world = this; //&b2world;
		b2world.SetDestructionListener(this);
		b2world.SetContactListener(this);

		timeStep = 1.f/60.f;
		velocityIterations = 8;
		positionIterations = 3;
		b2world.SetGravity(b2Vec2(0.0f, 9.8f));
		// Set to true, because this is the world itself
		m_inWorld = true;
	}


	void step()
	{
		dispatchStepEvent(true);
		b2world.Step(timeStep, velocityIterations, positionIterations);
		dispatchStepEvent(false);
	}


	b2World b2world;
	float32 timeStep;
	int32 velocityIterations;
	int32 positionIterations;
	b2Vec2 gravity;

	
	
	// Gets bodies under a specified point
	vector<sBody*> getBodiesAt(b2Vec2 position)
	{
		b2AABB aabb;
		aabb.lowerBound.Set(position.x, position.y);
		aabb.upperBound.Set(position.x, position.y);

		PointQueryCallback cb(position);
		b2world.QueryAABB(&cb, aabb);
		return cb.bodies;
	}
	vector<sBody*> getBodiesAABB(b2AABB aabb)
	{
		AABBQueryCallback cb;
		b2world.QueryAABB(&cb, aabb);
		return cb.bodies;
	}


	void setGravity(b2Vec2 grav)
	{
		b2world.SetGravity(grav);
	}

	//---------------------------------------------------------------------------------------
	// Manage listener for step events
	//---------------------------------------------------------------------------------------
	
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

	//---------------------------------------------------------------------------------------
	// Manage listener for contact events
	//---------------------------------------------------------------------------------------

	// If the last argument is omitted, contact with any other body will fire
	void addContactListener(sContactListener *listener, sBody *body1, sBody *body2 = nullptr)
	{
		if(body1 == body2) return;
		sContactPair contactPair = getContactPair(body1, body2);
		ContactMapType::value_type v(contactPair, listener);
		m_contactListeners.insert(v);
		//printf("add = %i \n", m_contactListeners.size());

	}
	void removeContactListener(sContactListener *listener, sBody *body1, sBody *body2 = NULL)
	{
		if(body1 == body2) return;
		sContactPair contactPair = getContactPair(body1, body2);
		//ContactMapType::value_type v(contactPair, listener);

		std::pair<ContactMapType::iterator, ContactMapType::iterator> range;
		range = m_contactListeners.equal_range(contactPair);
		for(ContactMapType::iterator i = range.first; i != range.second; ++i){
			if(i->second == listener){
				m_contactListeners.erase(i);
				printf("rem = %i \n", m_contactListeners.size());
				break;
			}
		}

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


	
	// b2DestructionListener impl
	void SayGoodbye(b2Joint * joint);
	void SayGoodbye(b2Fixture * fixture){}



	//-----------------------------------------------------------------------------------
	// World Query Callbacks
	//-----------------------------------------------------------------------------------


	class PointQueryCallback : public b2QueryCallback
	{
	public:
		PointQueryCallback(b2Vec2 position){
			m_position = position;
		}
		bool ReportFixture(b2Fixture* fixture){
			if(fixture->TestPoint(m_position)){
				bodies.push_back((sBody*)fixture->GetBody()->GetUserData());
				//return false;
			}
			return true;
		}
		vector<sBody*> bodies;
		b2Vec2 m_position;
	};

	
	class AABBQueryCallback : public b2QueryCallback
	{
	public:
		bool ReportFixture(b2Fixture* fixture){
			bodies.push_back((sBody*)fixture->GetBody()->GetUserData());
			return true;
		}
		vector<sBody*> bodies;
	};
	

	//-----------------------------------------------------------------------------------
	// Contact Events
	//-----------------------------------------------------------------------------------

	typedef multimap<sContactPair, sContactListener*, sContactPairCompare> ContactMapType;
	ContactMapType m_contactListeners;

public:
	void BeginContact (b2Contact *contact)
	{
		sBody *body1 = (sBody*)contact->GetFixtureA()->GetBody()->GetUserData();
		sBody *body2 = (sBody*)contact->GetFixtureB()->GetBody()->GetUserData();
		sContactPair contactPair = getContactPair(body1, body2);
		dispatchBeginContactEvent(contactPair);
		contactPair.body2 = nullptr;
		dispatchBeginContactEvent(contactPair);
	}
	void EndContact (b2Contact *contact)
	{
		sBody *body1 = (sBody*)contact->GetFixtureA()->GetBody()->GetUserData();
		sBody *body2 = (sBody*)contact->GetFixtureB()->GetBody()->GetUserData();
		sContactPair contactPair = getContactPair(body1, body2);
		dispatchEndContactEvent(contactPair);
		contactPair.body2 = nullptr;
		dispatchEndContactEvent(contactPair);
	};
	virtual void 	PreSolve (b2Contact *contact, const b2Manifold *oldManifold){};
	virtual void 	PostSolve (b2Contact *contact, const b2ContactImpulse *impulse){};

private:
	void dispatchBeginContactEvent(sContactPair contactPair)
	{
		std::pair<ContactMapType::iterator, ContactMapType::iterator> range;
		range = m_contactListeners.equal_range(contactPair);
		for(ContactMapType::iterator i = range.first; i != range.second; ++i){
			i->second->onBeginContact(contactPair);
		}
	}
	void dispatchEndContactEvent(sContactPair contactPair)
	{
		std::pair<ContactMapType::iterator, ContactMapType::iterator> range;
		range = m_contactListeners.equal_range(contactPair);
		for(ContactMapType::iterator i = range.first; i != range.second; ++i){
			i->second->onEndContact(contactPair);
		}
	}

	sContactPair getContactPair(sBody *body1, sBody *body2)
	{
		sContactPair contactPair;
		if(body2 == nullptr){
			contactPair.body1 = body1;
			contactPair.body2 = nullptr;
		} else if(body1 > body2){
			contactPair.body1 = body1;
			contactPair.body2 = body2;
		} else {
			contactPair.body1 = body2;
			contactPair.body2 = body1;
		}
		return contactPair;
	}
	

	//-----------------------------------------------------------------------------------
	// Step Events
	//-----------------------------------------------------------------------------------

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




