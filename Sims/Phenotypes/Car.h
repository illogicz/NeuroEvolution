#pragma once
#include "..\..\sPhysics\sPhysics.h"
#include "..\..\sPhysics\sWheelJoint.h"
#include "..\..\sEvolution\sPhenotype.h"

using std::vector;

class Car : public sPhenotype, public sContactListener
{

public:

	Car()
	{
	}
	static enum BodyType
	{
		Fixed,
		LowVariation,
		HighVariation
	};

	static BodyType bodyType;

protected:

	virtual void init(sWorld &world)
	{

		//---------------------------------------------------------------------------------
		// Phenotype Initialization
		//---------------------------------------------------------------------------------

		// Add bodyparts to container
		add(&frontWheel);
		add(&rearWheel);
		add(&chassis);
		add(&frontSuspension);
		add(&rearSuspension);


		// Filters out all collisions with parts and other cars
		b2Filter filter;
		filter.categoryBits = 0x02;
		filter.maskBits = 0x01;

		// Chassis
		chassis.setFilter(filter);
		chassis.setLinearDamping(0.1f);
		chassis.setAngularDamping(0.1f);

		// Front wheel
		frontWheel.setFilter(filter);
		frontWheel.setFriction(2.f);
		frontWheel.setRestitution(0.0f);
		frontWheel.setAngularDamping(0.1f);
		frontWheel.setLinearDamping(0.1f);

		// Rear wheel
		rearWheel.setFilter(filter);
		rearWheel.setFriction(2.f);
		rearWheel.setRestitution(0.0f);
		rearWheel.setAngularDamping(0.1f);
		rearWheel.setLinearDamping(0.1f);

		// Front suspension
		frontSuspension.setBodies(&chassis, &frontWheel);
		frontSuspension.setEnableMotor(true);

		// Rear suspension
		rearSuspension.setBodies(&chassis, &rearWheel);
		rearSuspension.setEnableMotor(true);

		// Add listeners for contact events
		world.addContactListener(this, &chassis);
		world.addContactListener(this, &frontWheel);
		world.addContactListener(this, &rearWheel);		
		
		//---------------------------------------------------------------------------------
		// Genome Definitions
		//---------------------------------------------------------------------------------

		if(bodyType == BodyType::HighVariation){

			// Chassis
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
			genome.addGene("frontFrequencyHz", 0.5, 6);
			genome.addGene("frontDampingRatio", 0.2, 0.999);

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
		
		
		} else if(bodyType == BodyType::Fixed) {

			// No variation */
			genome.addGene("scale", 1, 1);
			genome.addGene("chassisDensity", 2, 2);
			genome.addGene("chassisScale_x", 1.1, 1.1);
			genome.addGene("chassisScale_y", 1.1, 1.1);


			// Front Wheel
			genome.addGene("frontWheelRadius", 0.8, 0.8);
			genome.addGene("frontWheelDensity", 1, 1);
			genome.addGene("frontWheelPosition_x", 2,2);
			genome.addGene("frontWheelPosition_y", 1,1);
			genome.addGene("frontWheelAnchor_x", 1.3, 1.3);
			genome.addGene("frontWheelAnchor_y", -1 ,-1);

			// Front Motor
			genome.addGene("frontMotorTorque", 40, 40);
			genome.addGene("frontMotorSpeed", 70, 70);
		
			// Front Suspension
			genome.addGene("frontFrequencyHz", 2.5, 2.5);
			genome.addGene("frontDampingRatio", 0.8, 0.8);
		

			// Rear Wheel
			//genome.copyGene("rearWheelRadius", "frontWheelRadius");
			genome.addGene("rearWheelRadius", 1,1);
			genome.copyGene("rearWheelDensity", "frontWheelDensity");
			genome.copyGene("rearWheelPosition_x", "frontWheelPosition_x", GENE_INVERSE);
			genome.copyGene("rearWheelPosition_y", "frontWheelPosition_y");
			genome.copyGene("rearWheelAnchor_x", "frontWheelAnchor_x", GENE_INVERSE);
			genome.copyGene("rearWheelAnchor_y", "frontWheelAnchor_y");

			// Rear Motor
			genome.addGene("rearMotorTorque", 200, 200);
			genome.addGene("rearMotorSpeed", 150, 150);

			// Rear Suspension
			genome.copyGene("rearFrequencyHz", "frontFrequencyHz");
			genome.copyGene("rearDampingRatio", "frontDampingRatio");


		} else if(bodyType == BodyType::LowVariation) {

			// Small variation, generally valid phenotype
			genome.addGene("scale", 0.8, 1.2);
			genome.addGene("chassisDensity", 1.7, 1.7);
			genome.addGene("chassisScale_x", 0.9, 1.3);
			genome.addGene("chassisScale_y", 0.9, 1.3);


			// Front Wheel
			genome.addGene("frontWheelRadius", 0.5, 1.3);
			genome.addGene("frontWheelDensity", 1, 1);
			genome.addGene("frontWheelPosition_x", 1.3,2.2);
			genome.addGene("frontWheelPosition_y", 0.0, 1.5);
			genome.addGene("frontWheelAnchor_x", 1.2, 1.8);
			genome.addGene("frontWheelAnchor_y", -1.3 ,-0.0);

			// Front Motor
			genome.addGene("frontMotorTorque", 70, 70);
			genome.addGene("frontMotorSpeed", 40, 40);
		
			// Front Suspension
			genome.addGene("frontFrequencyHz", 2, 5);
			genome.addGene("frontDampingRatio", 0.6, 0.8);
		

			// Rear Wheel
			//genome.copyGene("rearWheelRadius", "frontWheelRadius");
			genome.addGene("rearWheelRadius", 0.6, 1.6);
			genome.copyGene("rearWheelDensity", "frontWheelDensity");
			genome.copyGene("rearWheelPosition_x", "frontWheelPosition_x", GENE_INVERSE);
			genome.copyGene("rearWheelPosition_y", "frontWheelPosition_y");
			genome.copyGene("rearWheelAnchor_x", "frontWheelAnchor_x", GENE_INVERSE);
			genome.copyGene("rearWheelAnchor_y", "frontWheelAnchor_y");

			// Rear Motor
			genome.addGene("rearMotorTorque", 200, 200);
			genome.addGene("rearMotorSpeed", 50, 50);

			// Rear Suspension
			genome.copyGene("rearFrequencyHz", "frontFrequencyHz");
			genome.copyGene("rearDampingRatio", "frontDampingRatio");
		
		}

		//---------------------------------------------------------------------------------
		// Neural Network Definition
		//---------------------------------------------------------------------------------
 
		neuralNet.setLayerCount(4);
		neuralNet.setNeuronLayer(0,12);
		neuralNet.setNeuronLayer(1,6);
		neuralNet.setNeuronLayer(2,3);
		neuralNet.setNeuronLayer(3,2);
		 

		//neuralNet.setWeightDistribution(0,3);
		//neuralNet.setWeightDistribution(1,2.5);
		//neuralNet.setWeightDistribution(2,1.5);
		/*
		neuralNet.setWeightDistribution(0,3.5);
		neuralNet.setWeightDistribution(1,2.5);
		neuralNet.setWeightDistribution(2,1.5);

		neuralNet.setUseFeedback(0,true);
		neuralNet.setMaxFeedback(2.f / tanh_approx(1));
		neuralNet.setFeedbackDistribution(0,2);


		//neuralNet.setHiddenLayerSize(1,4);
		neuralNet.setMaxBias(0.5f);
		neuralNet.setMaxWeight(2.f / tanh_approx(1));]
		*/


	}


	//=====================================================================================
	// sPhenotype implementation
	//=====================================================================================

	// Rebuilds the phenotype from it's genome
	void build(sWorld &world)
	{

		//---------------------------------------------------------------------------------
		// Phenotype Generation from Genome (Gene Expresion)
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

		// Reset neural net inputs
		neuralNet.update();


		// Initialise some values for simulation
		setThrottle(0,0);
		fitnessModifier = 1;
		progressPosition = chassis.getPosition().x;
		progressDelay = 0;
		deferDeath = false;
		frontWheelContact = false;
		rearWheelContact = false;
		lastLinearVelocity.Set(0,0);
		lastAngularVelocity = 0;

		if(isElite()){
			chassis.setCustomColor(b2Color(0,1,1));
		} else {
			chassis.setCustomColor(b2Color(0,0,0));
		}
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

	//---------------------------------------------------------------------------------
	// Phenotype Fitness Function
	//---------------------------------------------------------------------------------
	
	virtual float getFitness() 
	{
		float distance = chassis.getPosition().x + 10;
		//if(distance < 0)fitnessModifier = 0;
		float speed = abs(distance) / float(lifeTime + 1) * 60.f + 10.0f;
		//return float(pow(2, fitnessModifier * distance * speed * 0.1f)); //payloadMass;// / wheelMass;
		return fitnessModifier * distance * (speed); //payloadMass;// / wheelMass;
	}


	// Called before world physics is advanced
	void step()
	{
		assert(alive);

		if(deferDeath){
			die();
			deferDeath = false;
			return;
		}
		

		//---------------------------------------------------------------------------------
		// Neural Net behaviour
		//---------------------------------------------------------------------------------


		// INPUT NORMALIZATION

		int input_index = 0;

		neuralNet.interpolateInput(input_index++, neuralNet.getOutput(0), 0.5f);
		neuralNet.interpolateInput(input_index++, neuralNet.getOutput(1), 0.5f);

		// Set chassis angle input
		float angle = chassis.getAngle() / b2_pi;
		while(angle > 1)angle -= 2.f;
		while(angle < -1)angle += 2.f;
		neuralNet.setInput(input_index++, angle * b2_pi);

		// Set angular velocity input
		neuralNet.interpolateInput(input_index++, chassis.getAngularVelocity() * 0.3f, 0.5f);

		// Angular acceleration input
		neuralNet.interpolateInput(input_index++, getAngularAcceleration() * 1.f, 0.5f);

		// Front Wheel sensor input
		neuralNet.interpolateInput(input_index++, frontWheelContact ? 2.f : -0.f, 0.1f);

		// rear wheel sensor input
		neuralNet.interpolateInput(input_index++, rearWheelContact ? 2.f : -0.f, 0.1f);

		// Linear Velocity inputs
		b2Vec2 lv = chassis.getLinearVelocity();
		neuralNet.interpolateInput(input_index++, lv.x * 0.03f, 0.5f);
		neuralNet.interpolateInput(input_index++, lv.y * 0.05f, 0.5f);

		// Linear acceleration input
		b2Vec2 acceration = getLinearAcceleration();
		neuralNet.interpolateInput(input_index++, acceration.x, 0.5f);
		neuralNet.interpolateInput(input_index++, acceration.y, 0.5f);
		
		//if(isLeader()){
			//printf("%f \n", rearWheel.getAngularVelocity() / genome.getValue("rearMotorSpeed"));
		//}
		float rw_lv = 2.f * rearWheel.getAngularVelocity() / genome.getValue("rearMotorSpeed");
		neuralNet.interpolateInput(input_index++, rw_lv, 1);

	
		// Run neural net ////////////////////////////////
		neuralNet.run();
		//////////////////////////////////////////////////


		// OUTPUT

		int output_index = 0;
		//* Normal driving behaviour

		// Get outputs, these control the throttle
		float front_acc = neuralNet.getOutput(output_index++);
		float rear_acc = neuralNet.getOutput(output_index++);

		// Feedback
		//neuralNet.interpolateInput(input_index++, front_acc * 1.5f, 0.1f);

		//front_acc = front_acc * 0.75f + 0.25f;
		//rear_acc = rear_acc * 0.75f + 0.25f;

		// Color the wheel according to throttle values
		frontWheel.setCustomColor(getAccelatorColor(front_acc));
		rearWheel.setCustomColor(getAccelatorColor(rear_acc));

		// Apply throttle, ramp up slowly during the first 2 seconds
		// If removed they must figure it out themselves
		//if(lifeTime > 120){
		//	if(lifeTime <= 240){
		//		float f = float(lifeTime - 120) / 120.f;
		//		setThrottle(f * front_acc, f * front_acc);
		//	} else {
				setThrottle(front_acc, rear_acc);
		//	}
		//}
		//*/
		/*
		float frontAngle = b2_pi * neuralNet.getOutput(0);
		float rearAngle = b2_pi * neuralNet.getOutput(1);

		float front_acc = frontAngle - frontWheel.getAngle();
		float rear_acc = rearAngle - rearWheel.getAngle();

		setThrottle(0, 0);
		*/


		//float damping = neuralNet.getOutput(index++);
		//float freq = neuralNet.getOutput(index++);

		//rearSuspension.setDampingRatio((damping * 0.25f) + 0.65f);
		//frontSuspension.setDampingRatio((damping * 0.25f) + 0.65f);
		//rearSuspension.setFrequencyHz(freq * 2.f + 5.f);
		//frontSuspension.setFrequencyHz(freq * 2.f + 5.f);


		//---------------------------------------------------------------------------------
		// Death conditions
		//---------------------------------------------------------------------------------

		// check for joint instability
		b2Vec2 axis1 = chassis.m_body->GetLocalPoint(rearWheel.getPosition());
		axis1 -= rearSuspension.getAnchorA();
		b2Vec2 axis2 = chassis.m_body->GetLocalPoint(frontWheel.getPosition());
		axis2 -= frontSuspension.getAnchorA();
		if(axis1.Length() > 10.f || axis2.Length() > 10.f){
			printf("infant death\n");
			fitnessModifier = 0.4f;
			//genome.print();
			die();
		}

		// Kill off if it's upside down
		float a = chassis.getAngle();
		while(a > b2_pi)a -= b2_pi * 2;
		while(a < -b2_pi)a += b2_pi;
		if(abs(a) > b2_pi * 0.85){
			die();
			fitnessModifier = 0.1f;
			//return;
		}
		
		// Kill it off if it doesn't make progress for some time
		if(chassis.getPosition().x > progressPosition + 5.f){
			progressDelay = 0;
			progressPosition = chassis.getPosition().x;
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
		setCustomColor(b2Color(0,0,0));
		frontWheel.setType(STATIC_BODY);
		rearWheel.setType(STATIC_BODY);
		chassis.setType(STATIC_BODY);
	}



	// Body
	b2Vec2 frontAnchor;
	b2Vec2 rearAnchor;
	sConcavePolygon chassis;
	sCircle frontWheel;
	sCircle rearWheel;
	sWheelJoint frontSuspension;
	sWheelJoint rearSuspension;

	// Fitness modfiers
	float fallOverModifier;
	float breakModifier;
	float velocityModifier;
	float distanceModifier;

protected:

	
	float payloadMass;
	float wheelMass;
	static const int progressTimeout = 2000;
	int progressDelay;
	float progressPosition;
	float fitnessModifier;
	bool deferDeath;
	bool frontWheelContact;
	bool rearWheelContact;
	b2Vec2 lastLinearVelocity;
	float lastAngularVelocity;

	void setThrottle(float front, float rear)
	{
		//if(front < 0)front *= 0.5f;
		//if(rear < 0) rear *= 0.5f;

		//t *= payloadMass;
		frontSuspension.setMaxMotorTorque(abs(front) * genome.getValue("frontMotorTorque"));
		rearSuspension.setMaxMotorTorque(abs(rear) * genome.getValue("rearMotorTorque"));
		//frontSuspension.setMaxMotorTorque(genome.getValue("frontMotorTorque"));
		//rearSuspension.setMaxMotorTorque(genome.getValue("rearMotorTorque"));

		float c = frontSuspension.getMotorSpeed();
		float t = genome.getValue("frontMotorSpeed") * front;
		float v = c + (t - c) * 1.f;
		frontSuspension.setMotorSpeed(v);
		rearSuspension.setMotorSpeed(v);

	}

	b2Color getAccelatorColor(float acc)
	{
		if(acc > 0){
			return b2Color(1.f - acc, 1, 1.f - acc);
		} else {
			return b2Color(1, 1.f + acc, 1.f + acc);
		}
	}

	// Gets change in velocity since last time the function was called
	b2Vec2 getLinearAcceleration()
	{
		b2Vec2 velocity = chassis.getLinearVelocity();
		b2Vec2 dv = velocity - lastLinearVelocity;

		// remove gravity force ?
		//dv -= m_world->timeStep * m_world->gravity;

		lastLinearVelocity = velocity;
		return dv;
	}

	// Gets change in angular velocity since last time the function was called
	float getAngularAcceleration()
	{
		float av = chassis.getAngularVelocity();
		float dav = av - lastAngularVelocity;
		lastAngularVelocity = av;
		return dav;
	}

	void onBeginContact(sContactPair contactPair)
	{
		if(contactPair.contains(&chassis)) {
			if(alive){
				//fitnessModifier = 0.8f;
				//deferDeath = true;
			}
		} else if(contactPair.contains(&frontWheel)){
			frontWheelContact = true;
			//printf("front wheel touched \n");
		} else if(contactPair.contains(&rearWheel)){
			rearWheelContact = true;
		}
	}

	void onEndContact(sContactPair contactPair)
	{
		 if(contactPair.contains(&frontWheel)){
			 //printf("front wheel end contact \n");
			frontWheelContact = false;
		} else if(contactPair.contains(&rearWheel)){
			rearWheelContact = false;
		}
	}

};

Car::BodyType Car::bodyType = Car::BodyType::LowVariation;