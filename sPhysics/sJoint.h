#pragma once
#include <Box2D\Box2D.h>
#include "sBody.h"

class sJoint : public sObject
{
	friend class sWorld; // world must send destruction event
public:


	sJoint() : sObject(JOINT_OBJECT)
	{
		m_collideConnected = false;
	}

	void setBodies(sBody *bodyA, sBody *bodyB)
	{
		assert(!m_inWorld);

		m_bodyA = bodyA;
		m_bodyB = bodyB;
	}

	void setCollideConnected(bool collideConnected)
	{
		m_collideConnected = collideConnected;
	}


protected:


	void addToWorld(b2World &world)
	{
		sObject::addToWorld(world);

		// Objects must be added first
		//assert(m_bodyA->m_inWorld && m_bodyA->m_inWorld);


		m_jointDef->bodyA = m_bodyA->m_body;
		m_jointDef->bodyB = m_bodyB->m_body;
		m_jointDef->userData = this;
		m_jointDef->collideConnected = m_collideConnected;
		
		m_joint = world.CreateJoint(m_jointDef);
	}

	void removeFromWorld(b2World &world)
	{
		sObject::removeFromWorld(world);
		world.DestroyJoint(m_joint);
	}

	void jointDestroyed()
	{
		m_inWorld = false;
	}

	b2JointDef *m_jointDef;
	b2Joint *m_joint;

	sBody *m_bodyA;
	sBody *m_bodyB;

	bool m_collideConnected;
	

};


