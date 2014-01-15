#pragma once
#include <Box2D\Box2D.h>
#include "sBody.h"

class sBody;

class sJoint : public sObject
{
	friend class sWorld; // world must send destruction event
public:


	sJoint() : sObject(JOINT_OBJECT)
	{
		m_bodyA = m_bodyB = nullptr;
		m_collideConnected = false;
	}
	sJoint(sJoint &joint) : sObject(joint)
	{
		_copy(joint);
	}
	virtual void copy(sJoint &joint)
	{
		sObject::copy(joint);
		_copy(joint);
	}

	void setBodies(sBody *bodyA, sBody *bodyB)
	{
		m_bodyA = bodyA;
		m_bodyB = bodyB;
	}
	void setBodyA(sBody *bodyA)
	{
		m_bodyA = bodyA;
	}
	void setBodyB(sBody *bodyB)
	{
		m_bodyB = bodyB;
	}
	void setCollideConnected(bool collideConnected)
	{
		m_collideConnected = collideConnected;
	}


protected:


	virtual void addToWorld(sWorld &world);
	void _addToWorld(sWorld &world);
	virtual void removeFromWorld(sWorld &world);
	void _removeFromWorld(sWorld &world);


	void jointDestroyed()
	{
		m_jointWasDestroyed = true;
	}

	b2JointDef *m_jointDef;
	b2Joint *m_joint;

	sBody *m_bodyA;
	sBody *m_bodyB;

	bool m_collideConnected;

private:
	
	bool m_jointWasDestroyed;
	void _copy(sJoint &joint)
	{
		m_bodyA = joint.m_bodyA;
		m_bodyB = joint.m_bodyB;
		m_collideConnected = joint.m_collideConnected;
		m_jointDef = joint.m_jointDef;
	}

};


