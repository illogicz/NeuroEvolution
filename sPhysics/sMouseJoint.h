#pragma once
#include <Box2D\Box2D.h>
#include "sJoint.h"


class sMouseJoint : public sJoint
{
	//friend class sWorld; // world must send destruction event
public:

	sMouseJoint()
	{
		m_jointDef = &_jointDef;
	}



	void setMaxMotorTorque(float32 maxMotorTorque)
	{
		_jointDef.maxMotorTorque = maxMotorTorque;
		if(m_inWorld)_joint->SetMaxMotorTorque(maxMotorTorque);
	}


protected:

	void addToWorld(b2World &world)
	{
		sJoint::addToWorld(world);
		_joint = (b2MouseJoint*)&m_joint;
	}

	b2MouseJoint *_joint;
	b2MouseJointDef _jointDef;



};