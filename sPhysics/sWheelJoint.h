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
	sWheelJoint(sWheelJoint &joint) : sJoint(joint)
	{
		_copy(joint);
	}
	void copy(sWheelJoint &joint)
	{
		sJoint::copy(joint);
		_copy(joint);
	}

	void setAxis(b2Vec2 axis)
	{
		m_axis = axis;
		m_axis.Normalize();
	}

	b2Vec2 getAxis()
	{
		return m_axis;
	}

	void setAnchorA(b2Vec2 anchor)
	{
		m_localAnchorA = anchor;
	}
	b2Vec2 getAnchorA()
	{
		return m_localAnchorA;
	}

	void setAnchorB(b2Vec2 anchor)
	{
		m_localAnchorB = anchor;
	}
	b2Vec2 getAnchorB()
	{
		return m_localAnchorB;
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
		if(m_inWorld)_joint->EnableMotor(enableMotor);
	}

	void setFrequencyHz(float32 frequencyHz)
	{
		_jointDef.frequencyHz = frequencyHz;
		if(m_inWorld)_joint->SetSpringFrequencyHz(frequencyHz);
	}

	void setDampingRatio(float32 dampingRatio)
	{
		_jointDef.dampingRatio = dampingRatio;
		if(m_inWorld)_joint->SetSpringDampingRatio(dampingRatio);
	}

protected:

	void addToWorld(sWorld &world)
	{
		_jointDef.Initialize(m_bodyA->m_body, m_bodyB->m_body, m_localAnchorA, m_axis);
		sJoint::addToWorld(world);
		_joint = (b2WheelJoint*)m_joint;
	}


	b2Vec2 m_axis;
	b2Vec2 m_localAnchorA;
	b2Vec2 m_localAnchorB;

	b2WheelJoint *_joint;
	b2WheelJointDef _jointDef;

private:

	void _copy(sWheelJoint &joint)
	{
		m_jointDef = &_jointDef;
		_jointDef.maxMotorTorque = joint._jointDef.maxMotorTorque;
		_jointDef.motorSpeed = joint._jointDef.motorSpeed;
		_jointDef.enableMotor = joint._jointDef.enableMotor;
		_jointDef.frequencyHz = joint._jointDef.frequencyHz;
		_jointDef.dampingRatio = joint._jointDef.dampingRatio;
		m_axis = joint.m_axis;
		m_localAnchorB = joint.m_localAnchorB;
		m_localAnchorA = joint.m_localAnchorA;
	}

};