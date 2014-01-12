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


	void addToWorld(sWorld &world);
	void removeFromWorld(sWorld &world);


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


