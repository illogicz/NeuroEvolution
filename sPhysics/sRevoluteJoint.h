#pragma once
#include "sBody.h"
#include "sJoint.h"

class sRevoluteJoint : public sJoint
{
	//friend class sWorld; // world must send destruction event
public:

	sRevoluteJoint()
	{
		m_jointDef = &_jointDef;
	}
	sRevoluteJoint(sRevoluteJoint &joint) : sJoint(joint)
	{
		_copy(joint);
	}
	void copy(sRevoluteJoint &joint)
	{
		sJoint::copy(joint);
		_copy(joint);
	}

	void setAnchor(float x, float y)
	{
		setAnchor(b2Vec2(x,y));
	}
	void setAnchor(b2Vec2 globalAnchor)
	{
		m_globalAnchor = globalAnchor;
	}
	b2Vec2 getAnchor()
	{
		return m_globalAnchor;
	}

	void setMaxMotorTorque(float32 maxMotorTorque)
	{
		_jointDef.maxMotorTorque = maxMotorTorque;
		if(m_inWorld)_joint->SetMaxMotorTorque(maxMotorTorque);
	}
	float getMaxMotorTorque()
	{
		return _jointDef.maxMotorTorque;
	}

	float getMotorTorque()
	{
		if(m_inWorld)return _joint->GetMotorTorque(1);
		return 0;
	}

	void setMotorSpeed(float32 motorSpeed)
	{
		_jointDef.motorSpeed = motorSpeed;
		if(m_inWorld)_joint->SetMotorSpeed(motorSpeed);
	}
	float getMotorSpeed()
	{
		return _jointDef.motorSpeed;
	}

	void setEnableMotor(bool enableMotor)
	{
		_jointDef.enableMotor = enableMotor;
		if(m_inWorld)_joint->EnableMotor(enableMotor);
	}
	bool getEnabledMotor()
	{
		return _jointDef.enableMotor;
	}

	void setEnableLimit(bool enableLimit)
	{
		_jointDef.enableLimit = enableLimit;
		if(m_inWorld)_joint->EnableMotor(enableLimit);
	}	
	bool getEnableLimit()
	{
		return _jointDef.enableLimit;
	}

	void setLimits(float lowerAngle, float upperAngle)
	{
		_jointDef.lowerAngle = lowerAngle;
		_jointDef.upperAngle = upperAngle;
		if(m_inWorld)_joint->SetLimits(lowerAngle, upperAngle);
	}
	float getLowerLimit()
	{
		return _jointDef.lowerAngle;
	}
	float getUpperLimit()
	{
		return _jointDef.upperAngle;
	}

	float getAngle()
	{
		if(m_inWorld)return _joint->GetJointAngle();
		return 0.f;
	}

protected:

	void addToWorld(sWorld &world)
	{
		_jointDef.Initialize(m_bodyA->m_body, m_bodyB->m_body, m_globalAnchor);
		sJoint::addToWorld(world);
		_joint = (b2RevoluteJoint*)m_joint;
	}

	b2Vec2 m_globalAnchor;

	b2RevoluteJoint *_joint;
	b2RevoluteJointDef _jointDef;

private:

	void _copy(sRevoluteJoint &joint)
	{
		setAnchor(joint.getAnchor());
		setMaxMotorTorque(joint.getMaxMotorTorque());
		setMotorSpeed(joint.getMotorSpeed());
		setEnableMotor(joint.getEnabledMotor());
		setEnableLimit(joint.getEnableLimit());
		setLimits(joint.getLowerLimit(), joint.getUpperLimit());

	}

};