#pragma once
#include "..\..\sPhysics\sRectangle.h"
#include "..\..\sPhysics\sConvexPolygon.h"
#include "..\..\sPhysics\sRevoluteJoint.h"
#include "..\..\sEvolution\sPhenotype.h"


class TopDownCar : public sPhenotype
{

public:

	virtual void init(sWorld &world)
	{
	
		float width = 2;
		float height = 4;

		chassis.setSize(width,height);
		frontLeftWheel.setSize(0.3, 0.7);
		frontLeftWheel.setPosition(-width, -height);
		frontRightWheel.copy(frontLeftWheel);
		frontRightWheel.setPosition(width, -height);
		rearRightWheel.copy(frontLeftWheel);
		rearRightWheel.setPosition(-width, height);
		rearLeftWheel.copy(frontLeftWheel);
		rearLeftWheel.setPosition(width, height);


		frontLeftJoint.setAnchor(frontLeftWheel.getPosition());
		frontLeftJoint.setBodies(&chassis, &frontLeftWheel);
		frontLeftJoint.setEnableLimit(true);
		frontLeftJoint.setLimits(-0.6, 0.6);

		frontRightJoint.copy(frontLeftJoint);
		frontRightJoint.setAnchor(frontRightWheel.getPosition());
		frontRightJoint.setBodies(&chassis, &frontRightWheel);

		rearLeftJoint.setAnchor(rearLeftWheel.getPosition());
		rearLeftJoint.setBodies(&chassis, &rearLeftWheel);
		rearLeftJoint.setEnableLimit(true);
		rearLeftJoint.setLimits(0, 0);

		rearRightJoint.copy(rearLeftJoint);
		rearRightJoint.setAnchor(rearRightWheel.getPosition());
		rearRightJoint.setBodies(&chassis, &rearRightWheel);



		add(&frontLeftWheel);
		add(&frontRightWheel);
		add(&rearLeftWheel);
		add(&rearRightWheel);
	
		add(&frontLeftJoint);
		add(&frontRightJoint);
		add(&rearLeftJoint);
		add(&rearRightJoint);



	}

	virtual void build(sWorld &world)
	{

		chassis.setType(DYNAMIC_BODY);
		frontLeftWheel.setType(DYNAMIC_BODY);
		frontRightWheel.setType(DYNAMIC_BODY);
		rearRightWheel.setType(DYNAMIC_BODY);
		rearLeftWheel.setType(DYNAMIC_BODY);

	}
	virtual void destroy(sWorld &world)
	{
		chassis.setType(STATIC_BODY);
		frontLeftWheel.setType(STATIC_BODY);
		frontRightWheel.setType(STATIC_BODY);
		rearRightWheel.setType(STATIC_BODY);
		rearLeftWheel.setType(STATIC_BODY);
	}
	virtual void step() = 0;
	virtual b2Vec2 getPosition() = 0;
	virtual b2Vec2 getVelocity() = 0;
	virtual float getFitness()
	{
	    return (*fitnessFunction)(this);		
	};	



	sRectangle chassis;
	sRectangle frontLeftWheel;
	sRectangle frontRightWheel;
	sRectangle rearLeftWheel;
	sRectangle rearRightWheel;
	
	sRevoluteJoint frontLeftJoint;
	sRevoluteJoint frontRightJoint;
	sRevoluteJoint rearLeftJoint;
	sRevoluteJoint rearRightJoint;

	

private:

	sBodyState initialState;

	void applyWheelPhysics(sBody &body)
	{
		b2Vec2 v = b2Mul(b2Rot(-body.getAngle()), body.getLinearVelocity());

		float t = 1.0;
		if(v.y > 1) t = 0.5;
		v.y *= 1.f - t;

		body.setLinearVelocity(b2Mul(b2Rot(body.getAngle()), v));
		
	}
};