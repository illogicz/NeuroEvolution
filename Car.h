#pragma once
#include "sPhysics\sPhysics.h"
#include "sPhysics\sWheelJoint.h"

using std::vector;

class Car : public sContainer
{

public:

	Car()
	{

		m_wheel1.setRadius(0.5);
		m_wheel1.setPosition(b2Vec2(-1.5, 0.5));
		m_wheel1.setFriction(2);
		add(&m_wheel1);

		sCircle *m_wheel3 = new sCircle(m_wheel1);
		m_wheel3->setPosition(b2Vec2(1.5, 0.5));
		add(m_wheel3);

		//m_wheel2.copy(m_wheel1);
		//m_wheel2.setPosition(b2Vec2(1.5, 0.5));
		//add(&m_wheel2);

		m_car.add(2, 0);
		m_car.add(2, -0.5);
		m_car.add(1.5, -0.6);
		m_car.add(1, -1);
		m_car.add(-1, -1);
		m_car.add(-1.5, -0.6);
		m_car.add(-2, -0.5);
		m_car.add(-2, 0);
		m_car.finalizeShape();
		add(&m_car);


		b2Vec2 anchor1(-1.5, 0);
		b2Vec2 anchor2( 1.5, 0);

		m_wheelJoint1.setBodies(&m_car, &m_wheel1);
		m_wheelJoint1.setAnchorA(m_wheel1.getPosition());
		m_wheelJoint1.setAnchorB(anchor1);
		m_wheelJoint1.setAxis(anchor1 - m_wheel1.getPosition());
		m_wheelJoint1.setEnableMotor(true);
		m_wheelJoint1.setMotorSpeed(20);
		m_wheelJoint1.setMaxMotorTorque(10);
		add(&m_wheelJoint1);

		m_wheelJoint2.setBodies(&m_car, m_wheel3);
		m_wheelJoint2.setAnchorB(anchor2);
		m_wheelJoint2.setAnchorA(m_wheel3->getPosition());
		m_wheelJoint2.setAxis(anchor2 - m_wheel3->getPosition());
		m_wheelJoint2.setEnableMotor(true);
		m_wheelJoint2.setMotorSpeed(20);
		m_wheelJoint2.setMaxMotorTorque(10);
		add(&m_wheelJoint2);

	}


	void accelerate()
	{

	}

	void reverse()
	{

	}


	sConcavePolygon m_car;
	sCircle m_wheel1;
	//sCircle m_wheel2;
	sWheelJoint m_wheelJoint1;
	sWheelJoint m_wheelJoint2;



};