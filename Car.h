#pragma once
#include "sPhysics\sPhysics.h"
#include "sPhysics\sWheelJoint.h"

using std::vector;

class Car : public sContainer, public sStepListener
{

public:

	Car()
	{

		b2Filter filter;
		filter.categoryBits = 0x02;
		filter.maskBits = 0x01;


		maxMotorTorque = 20.f;
		maxMotorSpeed = 20.f;
		frontAnchor.Set(-1.5f, 0.f);
		rearAnchor.Set( 1.5f, 0.f);

		frontWheel.setRadius(0.75f);
		frontWheel.setPosition(b2Vec2(-1.7, 0.7));
		frontWheel.setFriction(1);
		frontWheel.setRestitution(0.0f);
		frontWheel.setAngularDamping(0.9);
		frontWheel.setDensity(0.25f);
		frontWheel.setFilter(filter);
		add(&frontWheel);

		rearWheel.copy(frontWheel);
		rearWheel.setPosition(b2Vec2(1.7, 0.7));
		rearWheel.setFriction(1);
		rearWheel.setRestitution(0.0f);
		rearWheel.setAngularDamping(0.9);
		rearWheel.setFilter(filter);
		add(&rearWheel);

		chassis.add(1.7f, 0.f);
		chassis.add(1.8f, -0.5f);
		chassis.add(1.5f, -0.6f);
		chassis.add(1.f, -1.f);
		chassis.add(-1.f, -1.f);
		chassis.add(-1.5f, -0.6f);
		chassis.add(-1.8f, -0.5f);
		chassis.add(-1.7f, 0.f);
		chassis.finalizeShape();
		chassis.setFilter(filter);
		add(&chassis);

		frontSuspension.setBodies(&chassis, &frontWheel);
		frontSuspension.setAnchorA(frontWheel.getPosition());
		frontSuspension.setAnchorB(frontAnchor);
		frontSuspension.setEnableMotor(true);
		frontSuspension.setMotorSpeed(0);
		frontSuspension.setMaxMotorTorque(5);
		frontSuspension.setFrequencyHz(3.f);
		frontSuspension.setDampingRatio(0.99f);
		add(&frontSuspension);


		rearSuspension.copy(frontSuspension);
		rearSuspension.setBodies(&chassis, &rearWheel);
		rearSuspension.setAnchorB(rearAnchor);
		rearSuspension.setAnchorA(rearWheel.getPosition());
		add(&rearSuspension);

	}


	void setAccelerator(float t)
	{
		
		frontSuspension.setMaxMotorTorque(abs(t) * maxMotorTorque);
		rearSuspension.setMaxMotorTorque(abs(t) * maxMotorTorque);
		frontSuspension.setMotorSpeed(maxMotorSpeed * t);
		rearSuspension.setMotorSpeed(maxMotorSpeed * t);
	}

	float32 maxMotorTorque;
	float32 maxMotorSpeed;
	b2Vec2 frontAnchor;
	b2Vec2 rearAnchor;
	sConcavePolygon chassis;
	sCircle frontWheel;
	sCircle rearWheel;
	sWheelJoint frontSuspension;
	sWheelJoint rearSuspension;


	void onBeforeStep()
	{

	}

	void onAfterStep()
	{

	}


protected:

	void addToWorld(sWorld &world)
	{
		rearSuspension.setAxis(rearAnchor - rearWheel.getPosition());
		frontSuspension.setAxis(frontAnchor - frontWheel.getPosition());
		sContainer::addToWorld(world);
		world.addStepListener(*this);
	}

	void removeFromWorld(sWorld &world)
	{
		world.removeStepListener(*this);
		sContainer::removeFromWorld(world);
	}

};