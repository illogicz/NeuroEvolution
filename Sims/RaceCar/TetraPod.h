#pragma once
#include "..\..\sUtils\sMath.h"
#include "..\..\sPhysics\sPhysics.h"
#include "..\..\sPhysics\sWheelJoint.h"
#include "..\..\sEvolution\sPhenotype.h"
#include "..\..\sPhysics\sRevoluteJoint.h"

using std::vector;

class TetraPod : public sPhenotype, public sContactListener
{

public:

	TetraPod()
	{
		jointTorque = 80.0;
		jointSpeed = 0.07;
		progressTimeout = 1000;
		footSensors = false;
		immortal = false;
		pulseFeedback = false;
		maxPulse = 0.2f;
		kinestetic = true;
		balanceSense = true;
		killOnStarvation = true;
		startEnergy = 10000;
	}

	bool killOnStarvation;
	float startEnergy;
	float energyLeft;
	float jointTorque;
	bool footSensors;
	float jointSpeed;
	bool pulseFeedback;
	float pulsePosition;
	float maxPulse;
	bool immortal;
	bool kinestetic;
	bool balanceSense;

protected:
	virtual void init(sWorld &world)
	{

		//---------------------------------------------------------------------------------
		// Phenotype Initialization
		//---------------------------------------------------------------------------------

		// Add bodyparts to container
		add(&torso);

		world.addContactListener(this, &torso);

		float s = 2;
		float torsoHeight = 0.3;
		torso.setPosition(1 * s,-1.45 * s);
		torso.addVertex(-1 * s, -0.2 * s);
		torso.addVertex(-1 * s,  0.2 * s);
		torso.addVertex( 1 * s,  0.2 * s);
		torso.addVertex( 1 * s, -0.2 * s);

		createLeg(world, 0, s);
		createLeg(world, 0, s);
		//createLeg(world, 2.f/3.f, s);
		//createLeg(world, 4.f/3.f, s);
		createLeg(world, 2, s);
		createLeg(world, 2, s);


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
		saveState();

		//---------------------------------------------------------------------------------
		// Genome Definitions
		//---------------------------------------------------------------------------------
		

		genome.addGene("maxMotorTorque", 80,100);
		genome.addGene("jointSpeed", 0.05, 0.05);
		genome.addGene("rearScale", 0.7, 1.4);
		genome.addGene("frontScale", 0.7, 1.4);
		genome.addGene("torsoScale", 0.5, 2);


		//---------------------------------------------------------------------------------
		// Neural Network Definition
		//---------------------------------------------------------------------------------
 
		int outputCount = m_joints.size();
		int inputCount = balanceSense ? 1 : 0;
		if(kinestetic)inputCount += m_joints.size();
		if(footSensors)inputCount += m_contactSensors.size();
		if(pulseFeedback){
			inputCount += 2;
			outputCount++;
		}
		neuralNet.setLayerCount(3);
		neuralNet.setNeuronLayer(0, inputCount, true, true);
		neuralNet.setNeuronLayer(1,outputCount, true, true);
		neuralNet.setNeuronLayer(2, outputCount, true, true);
		//neuralNet.addSynapseLayer(0,2);
		neuralNet.addSynapseLayer(0,2);

		//neuralNet.setHiddenLayerSize(0,(outputCount + inputCount) / 2);
		/*
		neuralNet.setMaxBias(0.5);
		neuralNet.setMaxWeight(1.f / tanh_approx(1));
		*/
	}


	sConvexPolygon* add(sConvexPolygon *bodyPart)
	{
		sContainer::add(bodyPart);
		m_bodies.push_back(bodyPart);
		return bodyPart;
	}
	sRevoluteJoint* add(sRevoluteJoint *joint)
	{
		sContainer::add(joint);
		m_joints.push_back(joint);
		return joint;
	}

	void createLeg(sWorld &world, float x, float s, float ao = 0.f, bool rev = false)
	{
		sConvexPolygon *leftUpperLeg = add(new sConvexPolygon);
		sConvexPolygon *leftLowerLeg = add(new sConvexPolygon);
	
		sRevoluteJoint *leftHip = add(new sRevoluteJoint);
		sRevoluteJoint *leftKnee = add(new sRevoluteJoint);

		leftLowerLeg->setPosition(x * s,-0.5 * s);
		leftLowerLeg->addVertex(-0.07 * s, 0.3 * s);
		leftLowerLeg->addVertex(-0.03 * s, 0.35 * s);
		leftLowerLeg->addVertex(0.03 * s, 0.35 * s);
		leftLowerLeg->addVertex(0.07 * s, 0.3 * s);
		leftLowerLeg->addVertex(0.12 * s, -0.3 * s);
		leftLowerLeg->addVertex(-0.12 * s, -0.3 * s);
		world.addContactListener(this, leftLowerLeg);
		m_contactSensors.push_back(leftLowerLeg);
		m_contactStates.push_back(false);

		leftUpperLeg->setPosition(x * s,-1.1 * s);
		leftUpperLeg->addVertex(-0.12 * s, 0.4 * s);
		leftUpperLeg->addVertex(0.12 * s, 0.4 * s);
		leftUpperLeg->addVertex(0.14 * s, -0.4 * s);
		leftUpperLeg->addVertex(-0.14 * s, -0.4 * s);

		leftKnee->setBodies(leftUpperLeg, leftLowerLeg);
		leftKnee->setAnchor(x * s,-0.75 * s);
		leftKnee->setEnableLimit(true);
		if(!rev){
			leftKnee->setLimits(-1,1);
		} else {
			leftKnee->setLimits(-1.5,0);
		}

		leftHip->setBodies(&torso, leftUpperLeg);
		leftHip->setAnchor(x * s,-1.4 * s);
		leftHip->setEnableLimit(true);
		if(!rev){
			leftHip->setLimits(-1,1);
		} else {
			leftHip->setLimits(0,1.5);
		}

		// Rotate from hip
		b2Rot a((leftHip->getUpperLimit() + leftHip->getLowerLimit()) / 2.f);
		float r = (leftHip->getUpperLimit() - leftHip->getLowerLimit()) / 2.f;

		b2Vec2 p = b2Mul(a, leftUpperLeg->getPosition() - leftHip->getAnchor());
		leftUpperLeg->setPosition(p + leftHip->getAnchor());
		leftUpperLeg->setAngle(a.GetAngle());
		leftHip->setLimits(-r, r);

		p = b2Mul(a, leftKnee->getAnchor() - leftHip->getAnchor());
		leftKnee->setAnchor(p + leftHip->getAnchor());

		p = b2Mul(a, leftLowerLeg->getPosition() - leftHip->getAnchor());
		leftLowerLeg->setPosition(p + leftHip->getAnchor());
		leftLowerLeg->setAngle(a.GetAngle());


		/// rotate from knee
		a.Set((leftKnee->getUpperLimit() + leftKnee->getLowerLimit()) / 2);
		r = (leftKnee->getUpperLimit() - leftKnee->getLowerLimit()) / 2.f;

		p = b2Mul(a, leftLowerLeg->getPosition() - leftKnee->getAnchor());
		leftLowerLeg->setPosition(p + leftKnee->getAnchor());
		leftLowerLeg->setAngle(a.GetAngle() + leftLowerLeg->getAngle());
		leftKnee->setLimits(-r, r);




		//leftUpperLeg->setPosition(b2Mul(t,leftUpperLeg->getPosition()));
		//leftUpperLeg->setAngle(a.GetAngle());
		//leftLowerLeg->setPosition(b2Mul(t,leftLowerLeg->getPosition()));
		//
		//leftKnee->setAnchor(b2Mul(t, leftKnee->getAnchor()));
		//

		//p = leftKnee->getAnchor();
		//a.Set(leftKnee->getUpperLimit() + leftKnee->getLowerLimit() / 2);
		//t.Set(p, a.GetAngle());
		//r = (leftKnee->getUpperLimit() - leftKnee->getLowerLimit()) / 2.f;

		//leftLowerLeg->setPosition(b2MulT(t,leftLowerLeg->getPosition()));
		//leftLowerLeg->setAngle(leftLowerLeg->getAngle() + a.GetAngle());
		//leftKnee->setLimits(-r, r);



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
		for(int i=0; i < m_bodies.size(); i++){
			m_bodies[i]->setType(DYNAMIC_BODY);
		}

		
		applySavedState();



		for(int i=0; i < m_joints.size(); i++){
			m_joints[i]->setMotorSpeed(0);
			m_joints[i]->setMaxMotorTorque(genome.getValue("maxMotorTorque"));
		}
		for(int i = 0; i < m_contactStates.size(); i++){
			m_contactStates[i] = false;
		}

		jointSpeed = genome.getValue("jointSpeed");;

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
		energyLeft = startEnergy;

		if(isElite()){
			setCustomColor(b2Color(0,1,1));
		} else {
			setCustomColor(b2Color(0,0,0));
		}
		rnd = sRandom::getFloat(1,2);
	}

	float rnd;
	virtual float getFitness()
	{
		float distance = torso.getPosition().x;
		float speed = 60.f * abs(distance) / float(lifeTime + 100);
		return distance < 3 ? rnd : (distance + 2.f);
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

		for(int i = 0; i < m_joints.size(); i++){
			energyLeft -= min(0.1f,abs(m_joints[i]->getMotorTorque()));
		}
		//if(isLeader())printf("%f \n", energyLeft);
		if(killOnStarvation && energyLeft <= 0){
			//printf("died from starvation \n");
			die();
			return;
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
		neuralNet.setInput(index++, angle * 3.f);

		if(kinestetic){
			for(int i = 0; i < m_joints.size(); i++){
				//float a = m_joints[i]->getAngle();
				//float mina = m_joints[i]->getLowerLimit();
				//float maxa = m_joints[i]->getUpperLimit();
				//float inp = (a - mina) / (maxa - mina) - 0.5;
				//if(isLeader())printf("%f \n", inp);
				//neuralNet.setInput(index++, a * 2.5f);
				float a = m_joints[i]->getAngle() / m_joints[i]->getUpperLimit();
				neuralNet.setInput(index++, a * 1.5f);

			}
		}
		if(footSensors){
			for(int i = 0; i < m_contactStates.size(); i++){
				float v = neuralNet.getInput(index);
				float g = m_contactStates[i] ? 1.f : -1.f;
				neuralNet.setInput(index++, v + (g - v) * 0.1f);
			}
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
			//o = o < 0 ? -sqrt(o) : sqrt(o);
			//o = sqrt(o);
			float ta = o * m_joints[i]->getUpperLimit();
			float a = m_joints[i]->getAngle(); // / ;
			float da = (ta - a);

			m_joints[i]->setMotorSpeed(da / m_world->timeStep * jointSpeed);
			//m_joints[i]->setMaxMotorTorque(10.f + abs(o - a) * 100.f);


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
		while(a < -b2_pi)a += b2_pi * 2;
		if(abs(a) > b2_pi * 0.9){
			die();
			//fitnessModifier = 0.9f;
			//return;
		}
		//if(isLeader()){
		//	printf("%f \n", getFitness());
		//}
		//if(alive)printf("%f %f\n", getFitness(), progressPosition);
		// Kill it off if it doesn't make progress for some time
		if(getFitness() > progressPosition + 0.5f){
			progressDelay = 0;
			progressPosition = getFitness();
		} else {
			if(++progressDelay > progressTimeout){
				//printf("timeout death\n");
				if(!immortal)die();
				return;
			}
		}
		

	}


	// Sets bodies to static
	void destroy(sWorld &world) // sPhenotype implementation
	{
		setCustomColor(b2Color(0,0,0));

		for(int i=0; i < m_bodies.size(); i++){
			m_bodies[i]->setType(STATIC_BODY);
		}

	}



	// Body
	sConvexPolygon torso;



	// Fitness modfiers
	float fallOverModifier;
	float velocityModifier;
	float distanceModifier;

protected:


	vector<sConvexPolygon*> m_bodies;
	vector<sRevoluteJoint*> m_joints;
	vector<sBodyState> m_initialStates;
	vector<sConvexPolygon*>m_contactSensors;
	vector<bool>m_contactStates;

	float progressTimeout;
	int progressDelay;
	float progressPosition;
	float fitnessModifier;
	bool deferDeath;
	bool rightFootContact;
	bool leftFootContact;
	b2Vec2 lastLinearVelocity;
	float lastAngularVelocity;


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
		if(contactPair.contains(&torso)){
			//deferDeath = true;
			return;
		}
		for(int i = 0; i < m_contactSensors.size(); i++){
			if(contactPair.contains(m_contactSensors[i])){
				m_contactStates[i] = true;
				break;
			}
		}
	}

	void onEndContact(sContactPair contactPair)
	{
		for(int i = 0; i < m_contactSensors.size(); i++){
			if(contactPair.contains(m_contactSensors[i])){
				m_contactStates[i] = false;
				break;
			}
		}
	}

};