#include "sJoint.h"


void sJoint::addToWorld(sWorld &world)
{
	sObject::addToWorld(world);
	_addToWorld(world);
}

void sJoint::_addToWorld(sWorld &world)
{
	if(m_bodyA == nullptr || m_bodyB == nullptr){
		printf("Error: sJoint::addToWorld() joint requires 2 bodies to be set\n");
		return;
	}
	if(!m_bodyA->isInWorld() || !m_bodyB->isInWorld()){
		printf("Error: sJoint::addToWorld() bodies of this joint are not all active\n");
		return;
	}
	m_jointDef->bodyA = m_bodyA->m_body;
	m_jointDef->bodyB = m_bodyB->m_body;
	m_jointDef->userData = this;
	m_jointDef->collideConnected = m_collideConnected;
		
	m_joint = world.b2world.CreateJoint(m_jointDef);

	m_jointWasDestroyed = false;
}

void sJoint::removeFromWorld(sWorld &world)
{
	_removeFromWorld(world);
	sObject::removeFromWorld(world);
}

void sJoint::_removeFromWorld(sWorld &world)
{
	if(m_inWorld && !m_jointWasDestroyed)world.b2world.DestroyJoint(m_joint);
}