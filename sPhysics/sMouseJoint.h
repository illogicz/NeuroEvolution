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
		m_draggingBody = false;
	}


	void setPosition(float32 x, float32 y)
	{
		if(m_inWorld){
			_joint->SetTarget(b2Vec2(x,y));
		}
	}

	void pressed(float32 x, float32 y)
	{
		if(m_inWorld){
			//sJoint::addToWorld(*m_world);
			//setPosition(x, y);
		}
	}
	void released(float32 x, float32 y)
	{
		setPosition(x, y);
	}

	/*
	void setMaxMotorTorque(float32 maxMotorTorque)
	{
		_jointDef.maxMotorTorque = maxMotorTorque;
		if(m_inWorld)_joint->SetMaxMotorTorque(maxMotorTorque);
	}
	*/

protected:

	// 
	void addToWorld(b2World &world)
	{
		
		//sJoint::addToWorld(world);
		//_joint = (b2MouseJoint*)&m_joint;
	}

	bool m_draggingBody;
	b2MouseJoint *_joint;
	b2MouseJointDef _jointDef;



};