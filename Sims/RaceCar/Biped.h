#pragma once
#include "..\..\sPhysics\sPhysics.h"
#include "..\..\sPhysics\sWheelJoint.h"
#include "..\..\sEvolution\sPhenotype.h"
#include "..\..\sPhysics\sRevoluteJoint.h"

using std::vector;

class Biped : public sPhenotype, public sContactListener
{

public:

	Biped()
	{
		jointTorque = 3.0;
		jointSpeed = 0.1;
		progressTimeout = 6000;
		footSensors = true;
		immortal = false;
		pulseFeedback = false;
		maxPulse = 0.1f;
		kinestetic = true;
		balanceSense = true;
	}

protected:

	float jointTorque;
	bool footSensors;
	float jointSpeed;
	bool pulseFeedback;
	float pulsePosition;
	float maxPulse;
	bool immortal;
	bool kinestetic;
	bool balanceSense;

	virtual void init(sWorld &world)
	{

		//---------------------------------------------------------------------------------
		// Phenotype Initialization
		//---------------------------------------------------------------------------------

		// Add bodyparts to container
		add(&torso);

		add(&leftUpperLeg);
		add(&leftLowerLeg);
		add(&leftFoot);
		//add(&leftUpperArm);
		//add(&leftLowerArm);
	
		//add(&leftShoulder);
		//add(&leftElbow);
		add(&leftHip);
		add(&leftKnee);
		add(&leftAnkle);

		add(&rightUpperLeg);
		add(&rightLowerLeg);
		add(&rightFoot);
		//add(&rightUpperArm);
		//add(&rightLowerArm);

		//add(&rightShoulder);
		//add(&rightElbow);
		add(&rightHip);
		add(&rightKnee);
		add(&rightAnkle);
		
		b2Filter filter;
		filter.categoryBits = 0x02;
		filter.maskBits = 0x01;
		for(int i=0; i < m_bodies.size(); i++){
			m_bodies[i]->setFilter(filter);
			m_bodies[i]->setRestitution(0);
			m_bodies[i]->setFriction(1);

		}
		for(int i=0; i < m_joints.size(); i++){
			m_joints[i]->setEnableMotor(true);
			m_joints[i]->setMaxMotorTorque(jointTorque);
		}
		// Filters out all collisions with parts and other cars


		leftFoot.addVertex(-0.3, -0.2);
		leftFoot.addVertex(-0.3, -0.05);
		leftFoot.addVertex(0.3, -0.05);
		leftFoot.addVertex(0.3, -0.1);
		leftFoot.addVertex(0.2, -0.2);
		rightFoot.copy(leftFoot);

		leftLowerLeg.setPosition(0,-0.5);
		leftLowerLeg.addVertex(-0.07, 0.35);
		leftLowerLeg.addVertex(0.07, 0.35);
		leftLowerLeg.addVertex(0.12, -0.3);
		leftLowerLeg.addVertex(-0.12, -0.3);
		rightLowerLeg.copy(leftLowerLeg);

		leftAnkle.setBodies(&leftFoot, &leftLowerLeg);
		leftAnkle.setAnchor(0,-0.15);
		leftAnkle.setEnableLimit(true);
		leftAnkle.setLimits(-1,1);
		leftAnkle.setMaxMotorTorque(jointTorque * 0.5);

		rightAnkle.copy(leftAnkle);
		rightAnkle.setBodies(&rightFoot, &rightLowerLeg);

		leftUpperLeg.setPosition(0,-1.1);
		leftUpperLeg.addVertex(-0.12, 0.4);
		leftUpperLeg.addVertex(0.12, 0.4);
		leftUpperLeg.addVertex(0.14, -0.4);
		leftUpperLeg.addVertex(-0.14, -0.4);
		rightUpperLeg.copy(leftUpperLeg);

		leftKnee.setBodies(&leftUpperLeg, &leftLowerLeg);
		leftKnee.setAnchor(0,-0.75);
		leftKnee.setEnableLimit(true);
		leftKnee.setLimits(-0.0,2);

		rightKnee.copy(leftKnee);
		rightKnee.setBodies(&rightUpperLeg, &rightLowerLeg);

		float torsoHeight = 0.3;
		torso.setPosition(0,-2);
		torso.addVertex(-0.14, 0.6);
		torso.addVertex(0.14, 0.6);
		torso.addVertex(0.175, -torsoHeight);

		torso.addVertex(0.1, -torsoHeight - 0.2);
		torso.addVertex(-0.1, -torsoHeight - 0.2);

		torso.addVertex(-0.175, -torsoHeight);

		leftHip.setBodies(&torso, &leftUpperLeg);
		leftHip.setAnchor(0,-1.4);
		leftHip.setEnableLimit(true);
		leftHip.setLimits(-1.6,0.5);

		rightHip.copy(leftHip);
		rightHip.setBodies(&torso, &rightUpperLeg);

		saveState();

		// Add listeners for contact events
		world.addContactListener(this, &torso);
		world.addContactListener(this, &leftFoot);
		world.addContactListener(this, &rightFoot);		
		world.addContactListener(this, &leftUpperLeg);
		world.addContactListener(this, &rightUpperLeg);		
		world.addContactListener(this, &leftLowerLeg);
		world.addContactListener(this, &rightLowerLeg);		
		
		//---------------------------------------------------------------------------------
		// Genome Definitions
		//---------------------------------------------------------------------------------



		// Small variation, generally valid phenotype
		genome.addGene("scale", 1, 1);

		

		//---------------------------------------------------------------------------------
		// Neural Network Definition
		//---------------------------------------------------------------------------------
 
		int outputCount = m_joints.size();
		int inputCount = balanceSense ? 1 : 0;
		if(kinestetic)inputCount += m_joints.size();
		if(footSensors)inputCount += 2;
		if(pulseFeedback){
			inputCount += 2;
			outputCount++;
		}
		neuralNet.setLayerCount(3);
		neuralNet.setLayerSize(0,inputCount);
		neuralNet.setLayerSize(1,outputCount);
		neuralNet.setLayerSize(2,outputCount);
		//neuralNet.setHiddenLayerSize(1,outputCount);
		//neuralNet.setHiddenLayerSize(1,4);
		//neuralNet.setMaxBias(0);
		//neuralNet.setMaxWeight(2);
		neuralNet.create(genome);

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
		torso.setType(DYNAMIC_BODY);

		leftUpperLeg.setType(DYNAMIC_BODY);
		leftLowerLeg.setType(DYNAMIC_BODY);
		leftFoot.setType(DYNAMIC_BODY);
		leftUpperArm.setType(DYNAMIC_BODY);

		rightUpperLeg.setType(DYNAMIC_BODY);
		rightLowerLeg.setType(DYNAMIC_BODY);
		rightFoot.setType(DYNAMIC_BODY);
		rightUpperArm.setType(DYNAMIC_BODY);
		
		applySavedState();


		for(int i=0; i < m_joints.size(); i++){
			m_joints[i]->setMotorSpeed(0);
		}


		// Reset neural net inputs
		neuralNet.update();


		// Initialise some values for simulation
		fitnessModifier = 1;
		progressPosition = getFitness();
		progressDelay = 0;
		deferDeath = false;
		leftFootContact = false;
		rightFootContact = false;
		lastLinearVelocity.Set(0,0);
		lastAngularVelocity = 0;
		pulsePosition = 0;
		
		if(isElite()){
			setCustomColor(b2Color(0,1,1));
		} else {
			setCustomColor(b2Color(0,0,0));
		}
		
	}

	virtual float getFitness()
	{
		return fitnessModifier * (getAABB().lowerBound.x + 5.f);
	}
	
	// Returns the position of this individual
	virtual b2Vec2 getPosition()
	{
		return torso.getPosition();
	}
	virtual b2Vec2 getVelocity()
	{
		return torso.getLinearVelocity();
	}

	void saveState()
	{
		m_initialStates.clear();
		for(int i=0; i < m_bodies.size(); i++){
			m_initialStates.push_back(m_bodies[i]->getState());
		}
	}
	void applySavedState()
	{
		for(int i=0; i < m_bodies.size(); i++){
			m_bodies[i]->setState(m_initialStates[i]);
		}
	}

	// Called before world physics is advanced
	void step()
	{
		assert(alive);

		if(deferDeath){
			deferDeath = false;
			if(!immortal){
				die();
				return;
			}
		}
		

		//---------------------------------------------------------------------------------
		// Neural Net behaviour
		//---------------------------------------------------------------------------------


		// INPUT NORMALIZATION

		int index = 0;

		// Set chassis angle input
		float angle = torso.getAngle() / b2_pi;
		while(angle > 1)angle -= 2.f;
		while(angle < -1)angle += 2.f;
		neuralNet.setInput(index++, angle);

		if(kinestetic){
			for(int i = 0; i < m_joints.size(); i++){
				//float a = m_joints[i]->getAngle();
				//float mina = m_joints[i]->getLowerLimit();
				//float maxa = m_joints[i]->getUpperLimit();
				//float inp = (a - mina) / (maxa - mina) - 0.5;
				//if(isLeader())printf("%f \n", inp);
				//neuralNet.setInput(index++, a * 2.5f);
				neuralNet.setInput(index++, m_joints[i]->getAngle());

			}
		}
		if(footSensors){
			// Front Wheel sensor input
			float v = neuralNet.getInput(index++);
			float g = leftFootContact ? 1.f : -1.f;
			neuralNet.setInput(index++, v + (g - v) * 0.2f);

			// rear wheel sensor input
			v = neuralNet.getInput(index++);
			g = rightFootContact ? 1.f : -1.f;
			neuralNet.setInput(index++, v + (g - v) * 0.2f);
		}

		if(pulseFeedback){
			neuralNet.setInput(index++, sin(pulsePosition));
			neuralNet.setInput(index++, cos(pulsePosition));
		}
		/*
		// Set angular velocity input
		neuralNet.setInput(index++, torso.getAngularVelocity() * 0.6f);

		// Angular acceleration input
		neuralNet.setInput(index++, getAngularAcceleration() * 10.f);



		// Linear Velocity inputs
		b2Vec2 lv = torso.getLinearVelocity();
		neuralNet.setInput(index++, lv.x * 0.05f);
		neuralNet.setInput(index++, lv.y * 0.05f);

		// Linear acceleration input
		b2Vec2 acceration = getLinearAcceleration();
		neuralNet.setInput(index++, acceration.x);
		neuralNet.setInput(index++, acceration.y);
		*/



		index = 0;
		// Run neural net ////////////////////////////////
		neuralNet.run();
		//////////////////////////////////////////////////


		// OUTPUT

		for(int i = 0; i < m_joints.size(); i++){
			//float o = (neuralNet.getOutput(index++) + 1.f) * 0.5f;
			float mina = m_joints[i]->getLowerLimit();
			float maxa = m_joints[i]->getUpperLimit();
			//float ta = o * (maxa - mina) + mina;
			//float ta = neuralNet.getOutput(index) * 2.f;
			//float ca = m_joints[i]->getAngle();

			//if(isLeader())printf("da %f \n", (ta - ca));

			//m_joints[i]->setMotorSpeed((ta - ca) / m_world->timeStep * jointSpeed);

			float o = neuralNet.getOutput(index++);
			float a = m_joints[i]->getAngle();
			m_joints[i]->setMotorSpeed((o * max(abs(mina), abs(maxa)) - a) / m_world->timeStep * jointSpeed);

		}
		if(pulseFeedback){
			pulsePosition += neuralNet.getOutput(index++) * maxPulse;
		}	
		//* Normal driving behaviour



		//---------------------------------------------------------------------------------
		// Death conditions
		//---------------------------------------------------------------------------------


		// Kill off if it's upside down
		float a = torso.getAngle();
		while(a > b2_pi)a -= b2_pi * 2;
		while(a < -b2_pi)a += b2_pi;
		if(abs(a) > b2_pi * 0.4){
			//die();
			//fitnessModifier = 0.9f;
			//return;
		}
		
		//if(alive)printf("%f %f\n", getFitness(), progressPosition);
		// Kill it off if it doesn't make progress for some time
		if(getFitness() > progressPosition + 0.01f){
			progressDelay = 0;
			progressPosition = getFitness();
		} else {
			if(++progressDelay > progressTimeout){
				if(!immortal)die();
				return;
			}
		}
		

	}


	// Sets bodies to static
	void destroy(sWorld &world) // sPhenotype implementation
	{
		setCustomColor(b2Color(0,0,0));

		torso.setType(STATIC_BODY);

		leftUpperLeg.setType(STATIC_BODY);
		leftLowerLeg.setType(STATIC_BODY);
		leftFoot.setType(STATIC_BODY);
		leftUpperArm.setType(STATIC_BODY);
		leftLowerArm.setType(STATIC_BODY);

		rightUpperLeg.setType(STATIC_BODY);
		rightLowerLeg.setType(STATIC_BODY);
		rightFoot.setType(STATIC_BODY);
		rightUpperArm.setType(STATIC_BODY);
		rightLowerArm.setType(STATIC_BODY);
	}



	// Body
	sConvexPolygon torso;

	sConvexPolygon leftUpperLeg;
	sConvexPolygon leftLowerLeg;
	sConvexPolygon leftFoot;
	sConvexPolygon leftUpperArm;
	sConvexPolygon leftLowerArm;
	
	sRevoluteJoint leftShoulder;
	sRevoluteJoint leftElbow;
	sRevoluteJoint leftHip;
	sRevoluteJoint leftKnee;
	sRevoluteJoint leftAnkle;


	sConvexPolygon rightUpperLeg;
	sConvexPolygon rightLowerLeg;
	sConvexPolygon rightFoot;
	sConvexPolygon rightUpperArm;
	sConvexPolygon rightLowerArm;

	sRevoluteJoint rightShoulder;
	sRevoluteJoint rightElbow;
	sRevoluteJoint rightHip;
	sRevoluteJoint rightKnee;
	sRevoluteJoint rightAnkle;

	// Fitness modfiers
	float fallOverModifier;
	float velocityModifier;
	float distanceModifier;

protected:


	vector<sConvexPolygon*> m_bodies;
	vector<sRevoluteJoint*> m_joints;
	vector<sBodyState> m_initialStates;
	
	float progressTimeout;
	int progressDelay;
	float progressPosition;
	float fitnessModifier;
	bool deferDeath;
	bool rightFootContact;
	bool leftFootContact;
	b2Vec2 lastLinearVelocity;
	float lastAngularVelocity;



	void add(sConvexPolygon *bodyPart)
	{
		sContainer::add(bodyPart);
		m_bodies.push_back(bodyPart);
	}
	void add(sRevoluteJoint *joint)
	{
		sContainer::add(joint);
		m_joints.push_back(joint);
	}

	// Gets change in velocity since last time the function was called
	b2Vec2 getLinearAcceleration()
	{
		b2Vec2 velocity = torso.getLinearVelocity();
		b2Vec2 dv = velocity - lastLinearVelocity;

		// remove gravity force ?
		dv -= m_world->timeStep * m_world->gravity;

		lastLinearVelocity = velocity;
		return dv;
	}

	// Gets change in angular velocity since last time the function was called
	float getAngularAcceleration()
	{
		float av = torso.getAngularVelocity();
		float dav = av - lastAngularVelocity;
		lastAngularVelocity = av;
		return dav;
	}

	void onBeginContact(sContactPair contactPair)
	{
		if(contactPair.contains(&leftFoot)){
			leftFootContact = true;
			//printf("front wheel touched \n");
		} else if(contactPair.contains(&rightFoot)){
			rightFootContact = true;
		} else {
			if(alive){
				fitnessModifier = 0.8f;
				deferDeath = true;
			}
		}
	}

	void onEndContact(sContactPair contactPair)
	{
		 if(contactPair.contains(&leftFoot)){
			 //printf("front wheel end contact \n");
			leftFootContact = false;
		} else if(contactPair.contains(&rightFoot)){
			rightFootContact = false;
		}
	}

};