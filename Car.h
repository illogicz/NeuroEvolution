#pragma once
#include "sPhysics\sPhysics.h"
#include "sPhysics\sWheelJoint.h"
#include "Genes\sGene.h"
using std::vector;

class Car : public sContainer, public sStepListener
{

public:

	Car()
	{
		add(&frontWheel);
		add(&rearWheel);
		add(&chassis);
		add(&frontSuspension);
		add(&rearSuspension);

		b2Filter filter;
		filter.categoryBits = 0x02;
		filter.maskBits = 0x01;

		frontSuspension.setBodies(&chassis, &frontWheel);
		frontSuspension.setEnableMotor(true);

		rearSuspension.setBodies(&chassis, &rearWheel);
		rearSuspension.setEnableMotor(true);

		frontWheel.setFilter(filter);
		rearWheel.setFilter(filter);
		chassis.setFilter(filter);


		// Define Genome

		addGene(g_maxMotorTorque.set(20, 5, 50));
		addGene(g_maxMotorSpeed.set(20, 5, 50));

		addGene(g_frontWheelRadius.set(1, 0.4, 1.6));
		addGene(g_frontWheelDensity.set(1, 0.5, 2));
		addGene(g_frontWheelPosition_x.set(-1.7, -2.5, -1));
		addGene(g_frontWheelPosition_y.set(0.7, 0.2, 2.5));
		addGene(g_frontWheelAnchor_x.set(-1.4, -1.4, -1.4));
		addGene(g_frontWheelAnchor_y.set(-0.5, -0.5 ,-0.5));

		addGene(g_frontFrequencyHz.set(2, 1, 3));
		addGene(g_frontDampingRatio.set(0.99, 0.5, 1));


		addGene(g_rearWheelRadius.copy(g_frontWheelRadius));
		addGene(g_rearWheelDensity.copy(g_frontWheelDensity));
		addGene(g_rearWheelPosition_x.set(1.7, 1, 2.5));
		addGene(g_rearWheelPosition_y.copy(g_frontWheelPosition_y));
		addGene(g_rearWheelAnchor_x.set(1.4, 1.4, 1.4));
		addGene(g_rearWheelAnchor_y.copy(g_frontWheelAnchor_y));

		addGene(g_rearFrequencyHz.copy(g_frontFrequencyHz));
		addGene(g_rearDampingRatio.copy(g_frontDampingRatio));
	}


	void mate(Car *p1, Car *p2)
	{
		for(int i = 0; i < genome.size(); i++){
			genome[i]->mate(p1->genome[i], p2->genome[i]);
		}
	}

	void randomize()
	{
		for(int i = 0; i < genome.size(); i++){
			genome[i]->random();
		}
	}


	void build()
	{

		// Apply Genome

		frontWheel.setType(DYNAMIC_BODY);
		rearWheel.setType(DYNAMIC_BODY);
		chassis.setType(DYNAMIC_BODY);

		maxMotorTorque = g_maxMotorTorque.getValue();
		maxMotorSpeed = g_maxMotorSpeed.getValue();
		frontAnchor.x = g_frontWheelAnchor_x.getValue();
		frontAnchor.y = g_frontWheelAnchor_y.getValue();
		rearAnchor.x = g_rearWheelAnchor_x.getValue();
		rearAnchor.y = g_rearWheelAnchor_y.getValue();

		frontWheel.zeroState();
		frontWheel.setRadius(g_frontWheelRadius.getValue());
		frontWheel.setPosition(b2Vec2(g_frontWheelPosition_x.getValue(), g_frontWheelPosition_y.getValue()));
		frontWheel.setFriction(1);
		frontWheel.setRestitution(0.0f);
		frontWheel.setAngularDamping(0.9);
		frontWheel.setDensity(g_frontWheelDensity.getValue());

		rearWheel.zeroState();
		rearWheel.setRadius(g_rearWheelRadius.getValue());
		rearWheel.setPosition(b2Vec2(g_rearWheelPosition_x.getValue(), g_rearWheelPosition_y.getValue()));
		rearWheel.setFriction(1);
		rearWheel.setRestitution(0.0f);
		rearWheel.setAngularDamping(0.9);
		rearWheel.setDensity(g_rearWheelDensity.getValue());

		chassis.zeroState();
		chassis.resetShape();
		chassis.add(1.7f, 0.f);
		chassis.add(1.8f, -0.5f);
		chassis.add(1.5f, -0.6f);
		chassis.add(1.f, -1.f);
		chassis.add(-1.f, -1.f);
		chassis.add(-1.5f, -0.6f);
		chassis.add(-1.8f, -0.5f);
		chassis.add(-1.7f, 0.f);
		chassis.finalizeShape();

		frontAnchor = b2Vec2(g_frontWheelAnchor_x.getValue(), g_frontWheelAnchor_y.getValue());
		frontSuspension.setAnchorA(frontWheel.getPosition());
		frontSuspension.setAnchorB(frontAnchor);
		frontSuspension.setMotorSpeed(0);
		frontSuspension.setFrequencyHz(g_frontFrequencyHz.getValue());
		frontSuspension.setDampingRatio(g_frontDampingRatio.getValue());


		rearAnchor = b2Vec2(g_rearWheelAnchor_x.getValue(), g_rearWheelAnchor_y.getValue());
		rearSuspension.setAnchorA(rearWheel.getPosition());
		rearSuspension.setAnchorB(rearAnchor);
		rearSuspension.setMotorSpeed(0);
		rearSuspension.setFrequencyHz(g_rearFrequencyHz.getValue());
		rearSuspension.setDampingRatio(g_rearDampingRatio.getValue());

		maxMotorTorque = g_maxMotorTorque.getValue();
		maxMotorSpeed = g_maxMotorSpeed.getValue();

		setAccelerator(0);

		alive = true;

		progressPosition = chassis.getPosition().x;
		progressDelay = 0;
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
		if(alive){
			float a = chassis.getAngle();
			while(a > b2_pi)a -= b2_pi * 2;
			while(a < -b2_pi)a += b2_pi;
			if(abs(a) > b2_pi * 0.7){
				die();
				return;
			}

			if(chassis.getPosition().x > progressPosition){
				progressDelay = 0;
				progressPosition = chassis.getPosition().x;
			} else {
				progressDelay++;
				if(progressDelay > progressTimeout){
					die();
					return;
				}
			}
		}
	}

	void onAfterStep()
	{

	}



	void die()
	{
		alive = false;
		//m_world->remove(this);
		frontWheel.setType(STATIC_BODY);
		rearWheel.setType(STATIC_BODY);
		chassis.setType(STATIC_BODY);
	}

	bool alive;

protected:

	static const int progressTimeout = 200;
	int progressDelay;
	float progressPosition;

	sGene g_maxMotorTorque;
	sGene g_maxMotorSpeed;

	sGene g_frontWheelRadius;
	sGene g_frontWheelDensity;
	sGene g_frontWheelPosition_x;
	sGene g_frontWheelPosition_y;
	sGene g_frontWheelAnchor_x;
	sGene g_frontWheelAnchor_y;

	sGene g_frontFrequencyHz;
	sGene g_frontDampingRatio;


	sGene g_rearWheelRadius;
	sGene g_rearWheelDensity;
	sGene g_rearWheelPosition_x;
	sGene g_rearWheelPosition_y;
	sGene g_rearWheelAnchor_x;
	sGene g_rearWheelAnchor_y;

	sGene g_rearFrequencyHz;
	sGene g_rearDampingRatio;

	vector<sGene*> genome;

	void addToWorld(sWorld &world)
	{
		build();
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

private:
	void addGene(sGene *gene)
	{
		genome.push_back(gene);
	}

};