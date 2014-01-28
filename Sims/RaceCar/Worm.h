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
		//thickness = 0.35f;
		thickness = 0.3f;
		maxJointAngle = 1.3f;

		// Movement Abilities
		muscleTorque = 20;
		muscleSpeed = 0.05f;
		maxPulse = 0.2f;
		lateralDamping = 0;

		// Neural Net settings
		maxNeuronBias = 2.0;
		maxSynapseWeight = 2.f / tanh(1);

		// Senses
		pulseFeedback = false;
		directFeedback = false;
		directionSense = false;
		bodySense = true;
		touchSense = true;
		contactGain = 0.1f;

		// Death Conditions
		killOnStarvation = false;
		startEnery = 3000;
		killOnLackOffProgress = true;
		progressTimeout = 1500;
		progressAmount = 1;
		

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
	bool touchSense;
	vector<float> additionNeuralInputs;
	vector<b2Vec2> directionTargets;
	




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

		
		for(int i = 0; i <= numSegments; i++){
			genome.addGene("WSW_" + to_string(i), 0.05, 0.35);
		}
		for(int i = 0; i < numSegments; i++){
			genome.addGene("WSL_" + to_string(i), 0.3, 1.4);
		}		
				
		

		//---------------------------------------------------------------------------------
		// Neural Network Definition
		//---------------------------------------------------------------------------------
 
		int inputCount = 0;
		int outputCount = numSegments - 1;
		if(pulseFeedback){
			inputCount += 2;
			outputCount++;
		}
		if(bodySense){ // as opposed to touch sense
			inputCount += numSegments - 1;
		}
		if(touchSense){
			inputCount += numSegments;
		}
		if(directionSense){
			inputCount += directionTargets.size();
		}
		neuralNet.setInputCount(inputCount);
		neuralNet.setOutputCount(outputCount);
		neuralNet.setHiddenLayerCount(2);
		neuralNet.setHiddenLayerSize(0,(inputCount + outputCount)/2);//inputCount);
		neuralNet.setHiddenLayerSize(1,(inputCount + outputCount)/2);//inputCount);
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

	float getSegmentThickness(int index)
	{
		//float(sin(float(index) / numSegments * b2_pi) * thickness * 0.4f + 0.15f);
		return genome.getValue("WSW_" + to_string(index)) / geneThicknessTotal * thickness * numSegments;
	}
	float getSegmentLength(int index)
	{
		//float(sin(float(index) / numSegments * b2_pi) * thickness * 0.4f + 0.15f);
		return (genome.getValue("WSL_" + to_string(index))) / geneLengthTotal * length;
	}
	float geneLengthTotal;
	float geneThicknessTotal;

	// Rebuilds the phenotype from it's genome
	void build(sWorld &world)
	{

		//---------------------------------------------------------------------------------
		// Phenotype Generation from Genome (Gene Expresion)
		//---------------------------------------------------------------------------------


		float x = -5.f;
		geneLengthTotal = 0;
		geneThicknessTotal = 0;
		for(int i = 0; i < numSegments; i++){
			geneLengthTotal += genome.getValue("WSL_" + to_string(i));
			geneThicknessTotal += genome.getValue("WSW_" + to_string(i));
		}
		for(int i = 0; i < numSegments; i++){


			m_segments[i]->setType(DYNAMIC_BODY);
			//m_segmentContact[i] = false;

			WormSegment* segment = m_segments[i];
			
			float h1 = getSegmentThickness(i + 1);
			float h2 = getSegmentThickness(i);


			 float w = getSegmentLength(i) / 2.f;
			//float w = length / numSegments * 0.5f;
			//float x = float(i) / numSegments * length - length * 0.5;

			if(i == 0){
				x -= w;
			} else {
				float w2 = getSegmentLength(i - 1) / 2.f;
				x -= w + w2;
				sRevoluteJoint* joint = m_joints[i-1];
				//float jx = float(i + 0.5f) / numSegments * length - length * 0.5;
				float jx = x + w;
				joint->setAnchor(jx,0);
				joint->setMotorSpeed(0);
			}



			segment->zeroState();
			segment->setPosition(x, 0);

			float s = sin(maxJointAngle / 2);
			float c = cos(maxJointAngle / 2);

			segment->clearVerices();
			segment->addVertex(-w, h1);
			segment->addVertex( w, h2);
			if(i == 0){
				float l = sqrt(0.5) / (sqrt(0.5) + 0.5) * h2;
				segment->addVertex( w + l, h2 - l);
				segment->addVertex( w + l, -h2 + l);
			} else {
				segment->addVertex( w + h2 * s, h2 * c);
				segment->addVertex( w + h2 * s, -h2 * c);
			}
			segment->addVertex( w,-h2);
			segment->addVertex(-w,-h1);
			if(i == numSegments - 1){
				float l = sqrt(0.5) / (sqrt(0.5) + 0.5) * h1;
				segment->addVertex(-w - l, -h1 + l);
				segment->addVertex(-w - l, h1 - l);
			} else {
				segment->addVertex(-w - h1 * s, -h1 * c);
				segment->addVertex(-w - h1 * s, h1 * c);
			}


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
		b2AABB aabb = getAABB();
		float y_mid = (getAABB().upperBound.y + getAABB().lowerBound.y) / 2.f;
		return b2Vec2(getAABB().upperBound.x, y_mid);
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
			//m_segments[i]->applyLateralDamping(lateralDamping);
		}
		if(killOnStarvation && eneryUsed > startEnery){
			printf("died from starvation \n");
			die();
			return;
		}

		//---------------------------------------------------------------------------------
		// Neural Net behaviour
		//---------------------------------------------------------------------------------


		// INPUT & NORMALIZATION

		int input_index = 0;
		if(touchSense){
			for(int i = 0; i < numSegments; i++){
				//neuralNet.setInput(i, m_segmentContact[i] ? 1.f : -1.f);
				neuralNet.interpolateInput(input_index++, m_segmentContact[i] ? 1.f : -1.f, contactGain);
			}
		}
		if(bodySense){
			for(int i = 0; i < numSegments - 1; i++){
				neuralNet.interpolateInput(input_index++, m_joints[i]->getAngle(), 0.5f);
			}
		}

		if(pulseFeedback){
			neuralNet.setInput(input_index++, sin(pulsePosition));
			neuralNet.setInput(input_index++, cos(pulsePosition));
		}

		if(directionSense){

			WormSegment &segment = *m_segments[m_segments.size()-1];
			float32 a = segment.getAngle();

			for(int i = 0; i < directionTargets.size(); i++){
				b2Vec2 td = directionTargets[i] - segment.getPosition();
				float32 ta = atan2(td.y, td.x);
				float32 da = ta - a;
				while(da > b2_pi)da -= b2_pi * 2;
				while(da < -b2_pi)da += b2_pi * 2;
				neuralNet.setInput(input_index++, da);
			}
		}
	
		// Run neural net ////////////////////////////////////////////////////
		neuralNet.run(); /////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////


		// OUTPUT

		int output_index = 0;
		for(int i = 0; i < numSegments - 1; i++){
			float o = neuralNet.getOutput(output_index++) * maxJointAngle;
			float a = m_joints[i]->getAngle();
			m_joints[i]->setMotorSpeed((o - a) / m_world->timeStep * muscleSpeed);
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