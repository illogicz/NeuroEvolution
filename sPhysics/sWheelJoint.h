#pragma once
#include <Box2D\Box2D.h>
#include "sBody.h"


class sWheelJoint : public sJoint
{
	//friend class sWorld; // world must send destruction event
public:

	sWheelJoint()
	{
		m_jointDef = &_jointDef;
	}



	void setAxis(b2Vec2 axis)
	{
		m_axis = axis;
	}

	void setAnchorA(b2Vec2 anchor)
	{
		m_localAnchorA = anchor;
	}

	void setAnchorB(b2Vec2 anchor)
	{
		m_localAnchorB = anchor;
	}

	void setMaxMotorTorque(float32 maxMotorTorque)
	{
		_jointDef.maxMotorTorque = maxMotorTorque;
		if(m_inWorld)_joint->SetMaxMotorTorque(maxMotorTorque);
	}

	void setMotorSpeed(float32 motorSpeed)
	{
		_jointDef.motorSpeed = motorSpeed;
		if(m_inWorld)_joint->SetMotorSpeed(motorSpeed);
	}

	void setEnableMotor(bool enableMotor)
	{
		_jointDef.enableMotor = enableMotor;
	}

protected:

	void addToWorld(sWorld &world)
	{

		_jointDef.Initialize(m_bodyA->m_body, m_bodyB->m_body, m_localAnchorA, m_axis);
		_jointDef.frequencyHz = 10;
		sJoint::addToWorld(world);
		_joint = (b2WheelJoint*)&m_joint;
	}


	b2Vec2 m_axis;
	b2Vec2 m_localAnchorA;
	b2Vec2 m_localAnchorB;

	b2WheelJoint *_joint;
	b2WheelJointDef _jointDef;



};