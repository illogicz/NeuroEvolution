#pragma once
#include "..\..\sPhysics\sPhysics.h"
#include "..\..\sPhysics\sWheelJoint.h"
#include "..\..\sEvolution\sPhenotype.h"
#include "..\..\sNeuralNet\sNeuralNet.h"
using std::vector;

class Car : public sPhenotype, public sContactListener
{

public:

	Car()
	{

		//---------------------------------------------------------------------------------
		// Phenotype Initialisation
		//---------------------------------------------------------------------------------

		// Add bodyparts to container
		add(&frontWheel);
		add(&rearWheel);
		add(&chassis);
		add(&frontSuspension);
		add(&rearSuspension);


		// Filters out all collisions
		b2Filter filter;
		filter.categoryBits = 0x02;
		filter.maskBits = 0x01;

		// Chais
		chassis.setFilter(filter);
		chassis.setLinearDamping(0.1f);
		chassis.setAngularDamping(0.1f);

		frontWheel.setFilter(filter);
		frontWheel.setFriction(1);
		frontWheel.setRestitution(0.0f);
		frontWheel.setAngularDamping(0.9);
		frontWheel.setLinearDamping(0.1);

		rearWheel.setFilter(filter);
		rearWheel.setFriction(1);
		rearWheel.setRestitution(0.0f);
		rearWheel.setAngularDamping(0.9);
		rearWheel.setLinearDamping(0.1);

		frontSuspension.setBodies(&chassis, &frontWheel);
		frontSuspension.setEnableMotor(true);

		rearSuspension.setBodies(&chassis, &rearWheel);
		rearSuspension.setEnableMotor(true);

		
		
		//---------------------------------------------------------------------------------
		// Genome Definition
		//---------------------------------------------------------------------------------

		// Chassis
		/*
		genome.addGene("scale", 0.7, 1.3);
		genome.addGene("chassisDensity", 1, 1);
		genome.addGene("chassisScale_x", 0.2, 2);
		genome.addGene("chassisScale_y", 0.5, 2.0);


		// Front Wheel
		genome.addGene("frontWheelRadius", 0.2, 1.5);
		genome.addGene("frontWheelDensity", 1, 1);
		genome.addGene("frontWheelPosition_x", -2.5, 2.5, 30);
		genome.addGene("frontWheelPosition_y", -2, 2, 30);
		genome.addGene("frontWheelAnchor_x", -1.8, 1.8, 30);
		genome.addGene("frontWheelAnchor_y", -1 ,0);

		// Front Motor
		genome.addGene("frontMotorTorque", 0, 300);
		genome.addGene("frontMotorSpeed", 0, 100);
		
		// Front Suspension
		genome.addGene("frontFrequencyHz", 0.2, 6);
		genome.addGene("frontDampingRatio", 0.2, 0.999);
		*/

		
		genome.addGene("scale", 1, 1);
		genome.addGene("chassisDensity", 1, 1);
		genome.addGene("chassisScale_x", 1, 1);
		genome.addGene("chassisScale_y", 1, 1);


		// Front Wheel
		genome.addGene("frontWheelRadius", 0.6, 0.6);
		genome.addGene("frontWheelDensity", 1, 1);
		genome.addGene("frontWheelPosition_x", 2,2);
		genome.addGene("frontWheelPosition_y", 1,1);
		genome.addGene("frontWheelAnchor_x", 1.5, 1.5);
		genome.addGene("frontWheelAnchor_y", -1 ,-1);

		// Front Motor
		genome.addGene("frontMotorTorque", 100, 100);
		genome.addGene("frontMotorSpeed", 100, 100);
		
		// Front Suspension
		genome.addGene("frontFrequencyHz", 3, 3);
		genome.addGene("frontDampingRatio", 0.7, 0.7);


		// Rear Wheel
		genome.copyGene("rearWheelRadius", "frontWheelRadius");
		genome.copyGene("rearWheelDensity", "frontWheelDensity");
		genome.copyGene("rearWheelPosition_x", "frontWheelPosition_x", GENE_INVERSE);
		genome.copyGene("rearWheelPosition_y", "frontWheelPosition_y");
		genome.copyGene("rearWheelAnchor_x", "frontWheelAnchor_x", GENE_INVERSE);
		genome.copyGene("rearWheelAnchor_y", "frontWheelAnchor_y");

		// Rear Motor
		genome.copyGene("rearMotorTorque", "frontMotorTorque");
		genome.copyGene("rearMotorSpeed", "frontMotorSpeed");

		// Rear Suspension
		genome.copyGene("rearFrequencyHz", "frontFrequencyHz");
		genome.copyGene("rearDampingRatio", "frontDampingRatio");

		neuralNet.setHiddenLayerCount(1);
		neuralNet.setInputCount(1);
		neuralNet.setOutputCount(1);
		neuralNet.setHiddenLayerCount(1);
		neuralNet.setHiddenLayerSize(0,4);
		neuralNet.create(genome);


	}


	//=====================================================================================
	// sPhenotype implementation
	//=====================================================================================


	void build(sWorld &world)
	{

		//---------------------------------------------------------------------------------
		// Phenotype Generation from Genome
		//---------------------------------------------------------------------------------

		// Make bodies dynamic again
		frontWheel.setType(DYNAMIC_BODY);
		rearWheel.setType(DYNAMIC_BODY);
		chassis.setType(DYNAMIC_BODY);

		// Get scale values
		float scale = genome.getValue("scale");
		float32 sx = genome.getValue("chassisScale_x") * scale;
		float32 sy = genome.getValue("chassisScale_y") * scale;

		// Set up chassis body
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

		// Front wheel
		frontWheel.zeroState();
		frontWheel.setRadius(genome.getValue("frontWheelRadius") * scale);
		frontWheel.setPosition(b2Vec2(genome.getValue("frontWheelPosition_x") * scale, 
			                          genome.getValue("frontWheelPosition_y") * scale));
		frontWheel.setDensity(genome.getValue("frontWheelDensity"));

		// Front suspension
		frontAnchor.x = genome.getValue("frontWheelAnchor_x") * sx;
		frontAnchor.y = genome.getValue("frontWheelAnchor_y") * sy;
		frontSuspension.setAnchorA(frontWheel.getPosition());
		frontSuspension.setAnchorB(frontAnchor);
		frontSuspension.setFrequencyHz(genome.getValue("frontFrequencyHz"));
		frontSuspension.setDampingRatio(genome.getValue("frontDampingRatio"));
		frontSuspension.setAxis(frontAnchor - frontWheel.getPosition());

		// Rear Wheel
		rearWheel.zeroState();
		rearWheel.setRadius(genome.getValue("rearWheelRadius") * scale);
		rearWheel.setPosition(b2Vec2(genome.getValue("rearWheelPosition_x") * scale, 
			                         genome.getValue("rearWheelPosition_y") * scale));
		rearWheel.setDensity(genome.getValue("rearWheelDensity"));

		// Rear suspension
		rearAnchor.x = genome.getValue("rearWheelAnchor_x") * sx;
		rearAnchor.y = genome.getValue("rearWheelAnchor_y") * sy;
		rearSuspension.setAnchorA(rearWheel.getPosition());
		rearSuspension.setAnchorB(rearAnchor);
		rearSuspension.setFrequencyHz(genome.getValue("rearFrequencyHz"));
		rearSuspension.setDampingRatio(genome.getValue("rearDampingRatio"));
		rearSuspension.setAxis(rearAnchor - rearWheel.getPosition());


		// Calculate values used in fitness function
		payloadMass = genome.getValue("chassisDensity") * sx * sy;
		wheelMass = genome.getValue("rearWheelDensity") * genome.getValue("rearWheelRadius") * scale * genome.getValue("rearWheelRadius") * scale;
		wheelMass += genome.getValue("frontWheelDensity") * genome.getValue("frontWheelRadius") * scale * genome.getValue("frontWheelRadius") * scale;

		// Initialise some values for simulation
		setAccelerator(0);
		fitnessModifier = 1;
		progressPosition = chassis.getPosition().x;
		progressDelay = 0;
		deferDeath = false;
	}

	
	// Returns the position of this individual
	virtual b2Vec2 getPosition()
	{
		return chassis.getPosition();
	}
	virtual b2Vec2 getVelocity()
	{
		return chassis.getLinearVelocity();
	}

	
	// Returns the fitness of the phenotype
	virtual float getFitness() 
	{
		float distance = chassis.getPosition().x;
		float speed = distance / float(lifeTime + 1);
		return fitnessModifier * distance * speed; //payloadMass;// / wheelMass;
	}


	// Called before world physics is advanced
	void step()
	{

		if(deferDeath){
			die();
			deferDeath = false;
			return;
		}

		// Start motors after 120 frames
		
			// TODO: Neural Net behaviour

		float input = chassis.getAngle() / b2_pi;
		while(input > 1)input -= 2.f;
		while(input < -1)input += 2.f;

		neuralNet.inputs[0].value = input;
		neuralNet.run();
		float output = neuralNet.outputs[0].activation();
		output += 1;
		output *= 0.5f;

		setCustomColor(b2Color(output, output, 1));

		if(lifeTime <= 120){
			setAccelerator(lifeTime / 240.f * output);
		} else {
			setAccelerator(output);
		}

		// check for joint instability
		b2Vec2 axis1 = chassis.m_body->GetLocalPoint(rearWheel.getPosition());
		axis1 -= rearSuspension.getAnchorA();
		b2Vec2 axis2 = chassis.m_body->GetLocalPoint(frontWheel.getPosition());
		axis2 -= frontSuspension.getAnchorA();
		if(axis1.Length() > 4.f || axis2.Length() > 4.f){
			printf("infant death\n");
			fitnessModifier = 0.0;
			//genome.print();
			die();
		}

		// Kill off if it's upside down
		
		float a = chassis.getAngle();
		while(a > b2_pi)a -= b2_pi * 2;
		while(a < -b2_pi)a += b2_pi;
		if(abs(a) > b2_pi * 0.7){
			die();
			fitnessModifier = 0.0f;
			return;
		}
		
		// Kill it off if it doesn't make progress for some time
		if(chassis.getPosition().x > progressPosition){
			progressDelay = 0;
			progressPosition = chassis.getPosition().x + 0.3f;
		} else {
			if(++progressDelay > progressTimeout){
				die();
				return;
			}
		}

	}


	// Sets bodies to static
	void destroy(sWorld &world) // sPhenotype implementation
	{
		frontWheel.setType(STATIC_BODY);
		rearWheel.setType(STATIC_BODY);
		chassis.setType(STATIC_BODY);
	}





	// Body Parts
	b2Vec2 frontAnchor;
	b2Vec2 rearAnchor;
	sConcavePolygon chassis;
	sCircle frontWheel;
	sCircle rearWheel;
	sWheelJoint frontSuspension;
	sWheelJoint rearSuspension;

protected:

	sNeuralNet neuralNet;
	float payloadMass;
	float wheelMass;
	static const int progressTimeout = 200;
	int progressDelay;
	float progressPosition;
	float fitnessModifier;
	bool deferDeath;

	void setAccelerator(float t)
	{
		//t *= payloadMass;
		frontSuspension.setMaxMotorTorque(abs(t) * genome.getValue("frontMotorTorque"));
		rearSuspension.setMaxMotorTorque(abs(t) * genome.getValue("rearMotorTorque"));
		frontSuspension.setMotorSpeed(genome.getValue("frontMotorSpeed") * t);
		rearSuspension.setMotorSpeed(genome.getValue("rearMotorSpeed") * t);
	}

	void onBeginContact(sContactPair contactPair)
	{
		if(alive){
			fitnessModifier = 0.0f;
			deferDeath = true;
		}
	}

};