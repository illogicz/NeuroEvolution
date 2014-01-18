#pragma once
#include "sPhysics\sPhysics.h"
#include "sPhysics\sWheelJoint.h"
#include "sEvolution\sGene.h"
#include "sEvolution\sGenome.h"
#include "sEvolution\sPhenotype.h"

using std::vector;

class Car : public sStepListener, public sPhenotype
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

		frontWheel.setFilter(filter);
		frontWheel.setFriction(1);
		frontWheel.setRestitution(0.0f);
		frontWheel.setAngularDamping(0.9);

		rearWheel.setFilter(filter);
		rearWheel.setFriction(1);
		rearWheel.setRestitution(0.0f);
		rearWheel.setAngularDamping(0.9);

		frontSuspension.setBodies(&chassis, &frontWheel);
		frontSuspension.setEnableMotor(true);

		rearSuspension.setBodies(&chassis, &rearWheel);
		rearSuspension.setEnableMotor(true);

		
		
		chassis.setFilter(filter);

		
		// Define Genome
		genome.addGene("scale", 0.7, 1.3);
		genome.addGene("chassisDensity", 1, 1);
		genome.addGene("chassisScale_x", 0.5, 1.5);
		genome.addGene("chassisScale_y", 0.5, 1.5);


		genome.addGene("frontMotorTorque", 0, 300);
		genome.addGene("frontMotorSpeed", 0, 300);
		
		genome.addGene("frontWheelRadius", 0.2, 1.5);
		genome.addGene("frontWheelDensity", 1, 1);
		genome.addGene("frontWheelPosition_x", 0.5, 2.5);
		genome.addGene("frontWheelPosition_y", -0.5, 2);
		genome.addGene("frontWheelAnchor_x", 0.1, 1.8);
		genome.addGene("frontWheelAnchor_y", -1 ,0);

		genome.addGene("frontFrequencyHz", 0.2, 6);
		genome.addGene("frontDampingRatio", 0.2, 0.999);

		genome.addGene("rearMotorTorque", 0,300);
		genome.addGene("rearMotorSpeed", 0,300);


		genome.copyGene("rearWheelRadius", "frontWheelRadius");
		genome.copyGene("rearWheelDensity", "frontWheelDensity");
		genome.copyGene("rearWheelPosition_x", "frontWheelPosition_x", GENE_INVERSE);
		genome.copyGene("rearWheelPosition_y", "frontWheelPosition_y");
		genome.copyGene("rearWheelAnchor_x", "frontWheelAnchor_x", GENE_INVERSE);
		genome.copyGene("rearWheelAnchor_y", "frontWheelAnchor_y");

		genome.copyGene("rearFrequencyHz", "frontFrequencyHz");
		genome.copyGene("rearDampingRatio", "frontDampingRatio");
	}

	void build()
	{

		// Apply Genome

		frontWheel.setType(DYNAMIC_BODY);
		rearWheel.setType(DYNAMIC_BODY);
		chassis.setType(DYNAMIC_BODY);
		float scale = genome.getValue("scale");
		float32 sx = genome.getValue("chassisScale_x") * scale;
		float32 sy = genome.getValue("chassisScale_y") * scale;

		frontAnchor.x = genome.getValue("frontWheelAnchor_x") * sx;
		frontAnchor.y = genome.getValue("frontWheelAnchor_y") * sy;
		rearAnchor.x = genome.getValue("rearWheelAnchor_x") * sx;
		rearAnchor.y = genome.getValue("rearWheelAnchor_y") * sy;

		frontWheel.zeroState();
		frontWheel.setRadius(genome.getValue("frontWheelRadius") * scale);
		frontWheel.setPosition(b2Vec2(genome.getValue("frontWheelPosition_x") * scale, 
			                          genome.getValue("frontWheelPosition_y") * scale));
		frontWheel.setDensity(genome.getValue("frontWheelDensity"));

		rearWheel.zeroState();
		rearWheel.setRadius(genome.getValue("rearWheelRadius") * scale);
		rearWheel.setPosition(b2Vec2(genome.getValue("rearWheelPosition_x") * scale, 
			                         genome.getValue("rearWheelPosition_y") * scale));
		rearWheel.setDensity(genome.getValue("rearWheelDensity"));

		chassis.zeroState();
		chassis.resetShape();
		chassis.setDensity(genome.getValue("chassisDensity"));
		
		chassis.add(1.7f * sx, 0.f * sy);
		chassis.add(1.8f * sx, -0.5f * sy);
		chassis.add(1.5f * sx, -0.6f * sy);
		chassis.add(1.f * sx, -1.f * sy);
		chassis.add(-1.f * sx, -1.f * sy);
		chassis.add(-1.5f * sx, -0.6f * sy);
		chassis.add(-1.8f * sx, -0.5f * sy);
		chassis.add(-1.7f * sx, 0.f * sy);
		chassis.finalizeShape();
		//s = 1;
		frontSuspension.setAnchorA(frontWheel.getPosition());
		frontSuspension.setAnchorB(frontAnchor);
		frontSuspension.setMotorSpeed(0);
		frontSuspension.setFrequencyHz(genome.getValue("frontFrequencyHz"));
		frontSuspension.setDampingRatio(genome.getValue("frontDampingRatio"));
		frontSuspension.setAxis(frontAnchor - frontWheel.getPosition());


		rearSuspension.setAnchorA(rearWheel.getPosition());
		rearSuspension.setAnchorB(rearAnchor);
		rearSuspension.setMotorSpeed(0);
		rearSuspension.setFrequencyHz(genome.getValue("rearFrequencyHz"));
		rearSuspension.setDampingRatio(genome.getValue("rearDampingRatio"));
		rearSuspension.setAxis(rearAnchor - rearWheel.getPosition());


		payloadMass = genome.getValue("chassisDensity") * sx * sy;

		wheelMass = genome.getValue("rearWheelDensity") * genome.getValue("rearWheelRadius") * scale * genome.getValue("rearWheelRadius") * scale;
		wheelMass += genome.getValue("frontWheelDensity") * genome.getValue("frontWheelRadius") * scale * genome.getValue("frontWheelRadius") * scale;


		setAccelerator(0);
		fitnessModifier = 1;
		alive = true;
		lifeTime = 0;
		progressPosition = chassis.getPosition().x;
		progressDelay = 0;

	}

	// sPhenotype implementation
	// Returns the fitness of the phenotype
	float getFitness()
	{
		float distance = chassis.getPosition().x;
		float speed = distance / float(lifeTime + 1);
		return fitnessModifier * distance * speed * payloadMass / wheelMass;
	}

	void setAccelerator(float t)
	{
		
		frontSuspension.setMaxMotorTorque(abs(t) * genome.getValue("frontMotorTorque"));
		rearSuspension.setMaxMotorTorque(abs(t) * genome.getValue("rearMotorTorque"));
		frontSuspension.setMotorSpeed(genome.getValue("frontMotorSpeed") * t);
		rearSuspension.setMotorSpeed(genome.getValue("rearMotorSpeed") * t);
	}

	void onBeforeStep()
	{

		if(alive){

			lifeTime++;

			// check for joint instability
			b2Vec2 axis1 = chassis.m_body->GetLocalPoint(rearWheel.getPosition());
			axis1 -= rearSuspension.getAnchorA();
			b2Vec2 axis2 = chassis.m_body->GetLocalPoint(frontWheel.getPosition());
			axis2 -= frontSuspension.getAnchorA();
			if(axis1.Length() > 4.f || axis2.Length() > 4.f){
				printf("infant death\n");
				fitnessModifier = 0.00;
				//genome.print();
				die();
			}

			// Kill off if it's upside down
			float a = chassis.getAngle();
			while(a > b2_pi)a -= b2_pi * 2;
			while(a < -b2_pi)a += b2_pi;
			if(abs(a) > b2_pi * 0.7){
				die();
				fitnessModifier = 0.0;
				return;
			}

			// Kill it off if it doesn't make progress for some time
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

	float payloadMass;
	float wheelMass;
	b2Vec2 frontAnchor;
	b2Vec2 rearAnchor;
	sConcavePolygon chassis;
	sCircle frontWheel;
	sCircle rearWheel;
	sWheelJoint frontSuspension;
	sWheelJoint rearSuspension;



protected:

	static const int progressTimeout = 200;
	int progressDelay;
	float progressPosition;
	float fitnessModifier;

	void addToWorld(sWorld &world)
	{
		build();
		sContainer::addToWorld(world);
		world.addStepListener(*this);
	}

	void removeFromWorld(sWorld &world)
	{
		world.removeStepListener(*this);
		sContainer::removeFromWorld(world);
	}

private:


};