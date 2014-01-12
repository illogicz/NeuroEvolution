#include "sJoint.h"


void sJoint::addToWorld(sWorld &world)
{
	sObject::addToWorld(world);
	_addToWorld(world);
}

void sJoint::_addToWorld(sWorld &world)
{
	m_jointDef->bodyA = m_bodyA->m_body;
	m_jointDef->bodyB = m_bodyB->m_body;
	m_jointDef->userData = this;
	m_jointDef->collideConnected = m_collideConnected;
		
	m_joint = world.b2world.CreateJoint(m_jointDef);
}

void sJoint::removeFromWorld(sWorld &world)
{
	sObject::removeFromWorld(world);
	_removeFromWorld(world);
}

void sJoint::_removeFromWorld(sWorld &world)
{
	world.b2world.DestroyJoint(m_joint);
}