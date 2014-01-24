#pragma once
#include "..\..\sPhysics\sPhysics.h"
#include "..\..\sPhysics\sRevoluteJoint.h"
#include "..\..\sEvolution\sPhenotype.h"
#include "WormSegment.h"

using std::vector;




class Worm : public sPhenotype, public sContactListener
{

public:

	Worm()
	{
		// Body Shape
		numSegments = 7;
		length = 5.f;
		thickness = 0.35f;
		maxJointAngle = 1.3f;

		// Movement Abilities
		muscleTorque = 10.f;
		muscleSpeed = 0.1f;
		maxPulse = 0.2f;
		lateralDamping = 0;

		// Neural Net settings
		maxNeuronBias = 0;
		maxSynapseWeight = 3;

		// Senses
		pulseFeedback = false;
		directFeedback = false;
		directionSense = false;
		bodySense = true;
		contactGain = 0.1f;

		// Death Conditions
		killOnStarvation = false;
		startEnery = 3000;
		killOnLackOffProgress = true;
		progressTimeout = 800;
		progressAmount = 0.0001;
		

	}

	~Worm()
	{
		for(int i = 0; i < numSegments; i++){
			m_world->removeContactListener(this, m_segments[i]);
			delete m_segments[i];

		}
		for(int i = 0; i < numSegments-1; i++){
			delete m_joints[i];
		}
	}

	float eneryUsed;

	bool killOnLackOffProgress;
	int progressTimeout;
	float startEnery;
	bool killOnStarvation;
	float progressAmount;
	// Body
	int numSegments;
	float length;
	float thickness;
	float maxJointAngle;
	float muscleTorque;
	float muscleSpeed;
	float maxPulse;
	float contactGain;
	float lateralDamping;

	// Brain
	float maxNeuronBias;
	float maxSynapseWeight;
	bool pulseFeedback;
	bool directFeedback;
	bool bodySense;
	bool directionSense;
	b2Vec2 directionTarget;




	virtual void init(sWorld &world)
	{

		//---------------------------------------------------------------------------------
		// Phenotype Initialization
		//---------------------------------------------------------------------------------

		// Add bodyparts to container

		b2Filter filter;
		filter.categoryBits = 0x02;
		filter.maskBits = 0x01;

		m_segments.resize(numSegments);
		m_segmentContact.resize(numSegments);
		for(int i = 0; i < numSegments; i++){

			m_segments[i] = new WormSegment;
			m_segments[i]->index = i;
			m_segments[i]->setFilter(filter);
			m_segments[i]->setFriction(1.f);
			m_segments[i]->setRestitution(0.f);
			world.addContactListener(this, m_segments[i]);
			add(m_segments[i]);

		}

		m_joints.resize(numSegments-1);
		for(int i = 0; i < numSegments-1; i++){
			sRevoluteJoint* joint = m_joints[i] = new sRevoluteJoint;
			joint->setBodies(m_segments[i], m_segments[i+1]);
			joint->setEnableLimit(true);
			joint->setEnableMotor(true);
			joint->setMaxMotorTorque(muscleTorque);
			joint->setLimits(-maxJointAngle, maxJointAngle);
			add(joint);
		}	

		//---------------------------------------------------------------------------------
		// Genome Definitions
		//---------------------------------------------------------------------------------

		
//		genome.addGene("num_segments", 0.7, 1.3);
				
		

		//---------------------------------------------------------------------------------
		// Neural Network Definition
		//---------------------------------------------------------------------------------
 
		int inputCount = numSegments;
		int outputCount = numSegments - 1;
		if(pulseFeedback){
			inputCount += 2;
			outputCount++;
		}
		if(bodySense){ // as opposed to touch sense
			inputCount--;
		}
		if(directionSense){
			inputCount++;
		}
		neuralNet.setInputCount(inputCount);
		neuralNet.setOutputCount(outputCount);
		neuralNet.setHiddenLayerCount(1);
		neuralNet.setHiddenLayerSize(0,inputCount);//inputCount);
		neuralNet.setMaxBias(maxNeuronBias);
		neuralNet.setMaxWeight(maxSynapseWeight);
		neuralNet.create(genome);

	}

	float getEneryUsed()
	{
		return eneryUsed;
	}


protected:
	//=====================================================================================
	// sPhenotype implementation
	//=====================================================================================

	// Rebuilds the phenotype from it's genome
	void build(sWorld &world)
	{

		//---------------------------------------------------------------------------------
		// Phenotype Generation from Genome (Gene Expresion)
		//---------------------------------------------------------------------------------


		for(int i = 0; i < numSegments; i++){


			m_segments[i]->setType(DYNAMIC_BODY);
			//m_segmentContact[i] = false;

			WormSegment* segment = m_segments[i];
			
			float h1 = float(sin(float(i) / numSegments * b2_pi) * thickness * 0.4f + 0.15f);
			float h2 = float(sin(float(i + 1) / numSegments * b2_pi) * thickness * 0.4 + 0.15f);
			float w = length / numSegments * 0.5f;
			float x = float(i) / numSegments * length - length * 0.5;

			segment->zeroState();
			segment->setPosition(x, 0);

			float s = sin(maxJointAngle / 2);
			float c = cos(maxJointAngle / 2);

			segment->clearVerices();
			segment->addVertex(-w, h1);
			segment->addVertex( w, h2);
			if(i == numSegments - 1){
				float l = sqrt(0.5) / (sqrt(0.5) + 0.5) * h2;
				segment->addVertex( w + l, h2 - l);
				segment->addVertex( w + l, -h2 + l);
			} else {
				segment->addVertex( w + h2 * s, h2 * c);
				segment->addVertex( w + h2 * s, -h2 * c);
			}
			segment->addVertex( w,-h2);
			segment->addVertex(-w,-h1);
			if(i == 0){
				float l = sqrt(0.5) / (sqrt(0.5) + 0.5) * h1;
				segment->addVertex(-w - l, -h1 + l);
				segment->addVertex(-w - l, h1 - l);
			} else {
				segment->addVertex(-w - h1 * s, -h1 * c);
				segment->addVertex(-w - h1 * s, h1 * c);
			}
		}

		for(int i = 0; i < numSegments-1; i++){
			sRevoluteJoint* joint = m_joints[i];
			float x = float(i + 0.5f) / numSegments * length - length * 0.5;
			joint->setAnchor(x,0);
			joint->setMotorSpeed(0);
		}
		

		neuralNet.prepare();


		// Initialise some values for simulation
		fitnessModifier = 1;
		progressPosition = getFitness();
		progressDelay = 0;
		deferDeath = false;
		pulsePosition = 0.f;
		feedBackValue = 0.f;
		eneryUsed = 0.f;

		if(isElite()){
			setCustomColor(b2Color(0,1,1));
		} else {
			setCustomColor(b2Color(0,0,0));
		}
	}

	
	// Returns the position of this individual
	virtual b2Vec2 getPosition()
	{
		return m_segments[int(numSegments/2)]->getPosition();
	}
	virtual b2Vec2 getVelocity()
	{
		return m_segments[int(numSegments/2)]->getLinearVelocity();
	}

	//---------------------------------------------------------------------------------
	// Phenotype Fitness Function
	//---------------------------------------------------------------------------------
	
	// Called before world physics is advanced
	void step()
	{
		assert(alive);

		if(deferDeath){
			die();
			deferDeath = false;
			return;
		}
		

		for(int i = 0; i < numSegments-1; i++){
			eneryUsed += abs(m_joints[i]->getMotorTorque());
		}
		for(int i = 0; i < numSegments; i++){
			m_segments[i]->applyLateralDamping(lateralDamping);
		}
		if(killOnStarvation && eneryUsed > startEnery){
			printf("died from starvation \n");
			die();
			return;
		}

		//---------------------------------------------------------------------------------
		// Neural Net behaviour
		//---------------------------------------------------------------------------------


		// INPUT NORMALIZATION

		int input_index = 0;
		if(!bodySense){
			for(int i = 0; i < numSegments; i++){
				//neuralNet.setInput(i, m_segmentContact[i] ? 1.f : -1.f);
				neuralNet.interpolateInput(input_index++, m_segmentContact[i] ? 1.f : -1.f, contactGain);
			}
		} else {
			for(int i = 0; i < numSegments - 1; i++){
				neuralNet.setInput(input_index++, m_joints[i]->getAngle());
			}
		}
		if(pulseFeedback){
			neuralNet.setInput(input_index++, sin(pulsePosition));
			neuralNet.setInput(input_index++, cos(pulsePosition));
		}
		if(directionSense){

			WormSegment &segment = *m_segments[m_segments.size()-1];
			float32 a = segment.getAngle();
			b2Vec2 td = directionTarget - segment.getPosition();
			float32 ta = atan2(td.y, td.x);
			float32 da = ta - a;
			while(da > b2_pi)da -= b2_pi * 2;
			while(da < -b2_pi)da += b2_pi * 2;

			neuralNet.setInput(input_index++, da);

		}
	
		// Run neural net ////////////////////////////////////////////
		neuralNet.run();



		// OUTPUT
		int output_index = 0;
		for(int i = 0; i < numSegments - 1; i++){
			float o = neuralNet.getOutput(output_index++);
			float a = m_joints[i]->getAngle();
			m_joints[i]->setMotorSpeed((o * maxJointAngle - a) / m_world->timeStep * muscleSpeed);
		}
		if(pulseFeedback){
			pulsePosition += neuralNet.getOutput(output_index++) * maxPulse;
		}	


		//---------------------------------------------------------------------------------
		// Death conditions
		//---------------------------------------------------------------------------------

		
		// Kill it off if it doesn't make progress for some time
		if(killOnLackOffProgress){
			if(getFitness() > progressPosition + progressAmount){
				progressDelay = 0;
				progressPosition = getFitness();
			} else {
				if(++progressDelay > progressTimeout){
					die();
					return;
				}
			}
		}
	}


	// Sets bodies to static
	void destroy(sWorld &world) // sPhenotype implementation
	{
		setCustomColor(b2Color(0,0,0));
		for(int i = 0; i < numSegments; i++){
			m_segments[i]->setType(STATIC_BODY);
		}
		//chassis.setType(STATIC_BODY);
	}



	// Body
	vector<WormSegment*> m_segments;
	vector<sRevoluteJoint*> m_joints;
	vector<bool> m_segmentContact;


protected:

	int progressDelay;
	float progressPosition;
	float fitnessModifier;
	bool deferDeath;
	float pulsePosition;
	float feedBackValue;

	void onBeginContact(sContactPair contactPair)
	{
		for(int i = 0; i < numSegments; i++){
			if(contactPair.contains(m_segments[i])){
				m_segmentContact[i] = true;
				return;
			}
		}
	}

	void onEndContact(sContactPair contactPair)
	{
		for(int i = 0; i < numSegments; i++){
			if(contactPair.contains(m_segments[i])){
				m_segmentContact[i] = false;
				return;
			}
		}
	}

};