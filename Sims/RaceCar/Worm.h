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

	
	//---------------------------------------------------------------------------------
	// Worm Settings, static, for now?
	//---------------------------------------------------------------------------------

	// Death Conditions
	static bool killOnLackOffProgress;
	static int progressTimeout;
	static float progressAmount;
	static int maxLifeTime;

	static float startEnery;
	static bool killOnStarvation;
	static float baseMetabolism;

	// Body
	static int numSegments;
	static float length;
	static float thickness;
	static float maxJointAngle;
	static float muscleTorque;
	static float muscleSpeed;
	static float maxPulse;
	static float contactGain;
	static float lateralDamping;
	
	// Senses
	static bool pulseFeedback;
	static bool directFeedback;
	static bool bodySense;
	static bool directionSense;
	static bool touchSense;
	static bool orientationSense;

	// Muscles
	static bool muscleModel;


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
			if(touchSense){
				world.addContactListener(this, m_segments[i]);
			}
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
			//genome.addGene("WSW_" + to_string(i), 0.07, 0.35);
			genome.addGene("WSW_" + to_string(i), 0.35, 0.35);
		}
		for(int i = 0; i < numSegments; i++){
			//genome.addGene("WSL_" + to_string(i), 1, 3);
			genome.addGene("WSL_" + to_string(i), 1, 1);
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
		if(bodySense){
			inputCount += numSegments - 1;
		}
		if(orientationSense){
			inputCount++;
		}
		if(touchSense){
			inputCount += numSegments;
		}
		if(directionSense){
			inputCount += directionTargets.size();
		}
		if(muscleModel){
			outputCount += numSegments - 1;
		}
		neuralNet.setLayerCount(4);
		neuralNet.setLayerSize(0,inputCount);
		neuralNet.setLayerSize(1,(inputCount + outputCount)/2);//inputCount);
		neuralNet.setLayerSize(2,(inputCount + outputCount)/2);//inputCount);
		neuralNet.setLayerSize(3,outputCount);
		
		/*

		neuralNet.setWeightDistribution(0,7);
		neuralNet.setWeightDistribution(1,8);
		neuralNet.setWeightDistribution(2,9);

		neuralNet.setUseFeedback(0,false);
		neuralNet.setUseFeedback(1,false);
		neuralNet.setMaxFeedback(1);
		neuralNet.setFeedbackDistribution(0,3);
		neuralNet.setFeedbackDistribution(1,3);

		//neuralNet.setHiddenLayerSize(1,(inputCount + outputCount)/2);//inputCount);
		neuralNet.setMaxBias(0.3);
		neuralNet.setMaxWeight(3);

		*/

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
		return 0.15f;
		//return float(sin(float(index) / numSegments * b2_pi) * thickness * 0.4f + 0.15f);
		//return genome.getValue("WSW_" + to_string(index)) / geneThicknessTotal * thickness * numSegments;
	}
	float getSegmentLength(int index)
	{
		return (genome.getValue("WSL_" + to_string(index))) / geneLengthTotal * length;
	}
	float geneLengthTotal;
	float geneThicknessTotal;

	// Rebuilds the phenotype from it's genome
	void build(sWorld &world)
	{

		//---------------------------------------------------------------------------------
		// Phenotype Generation from Genome (Gene Expression)
		//---------------------------------------------------------------------------------


		float x = 0;
		geneLengthTotal = 0;
		geneThicknessTotal = 0;
		for(int i = 0; i < numSegments; i++){
			geneLengthTotal += genome.getValue("WSL_" + to_string(i));
			geneThicknessTotal += genome.getValue("WSW_" + to_string(i));
		}
		for(int i = 0; i < numSegments; i++){

			m_segmentContact[i] = false;
			if(i == numSegments - 1){
				m_segments[i]->setType(STATIC_BODY);
			} else {
				m_segments[i]->setType(DYNAMIC_BODY);
			}
			//m_segmentContact[i] = false;

			WormSegment* segment = m_segments[i];
			
			float h1 = getSegmentThickness(i + 1);
			float h2 = getSegmentThickness(i);


			 float w = getSegmentLength(i) / 2.f;
			//float w = length / numSegments * 0.5f;
			//float x = float(i) / numSegments * length - length * 0.5;

			if(i == 0){
				float l = sqrt(0.5) / (sqrt(0.5) + 0.5) * h2;
				x -= w + l;
			} else {
				float w2 = getSegmentLength(i - 1) / 2.f;
				x -= w + w2;
				sRevoluteJoint* joint = m_joints[i-1];
				//float jx = float(i + 0.5f) / numSegments * length - length * 0.5;
				float jx = x + w;
				joint->setAnchor(jx,0);
				joint->setMotorSpeed(0);
				if(muscleModel){
					joint->setMaxMotorTorque(0);
				}
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
	
		// Zero inputs and update weight and biases from genes
		neuralNet.update();


		// Reset some values for this run
		fitnessModifier = 1;
		totalHeight = 0;
		progressPosition = 0; //getProgress();
		progressDelay = 0;
		deferDeath = false;
		pulsePosition = 0.f;
		feedBackValue = 0.f;
		eneryUsed = 0.f;
		total_x = 0;
		// Set a custom color for elites
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
		float y_mid = (aabb.upperBound.y + aabb.lowerBound.y) / 2.f;
		//float x_mid = (getAABB().upperBound.x + getAABB().lowerBound.x) / 2.f;
		return b2Vec2(aabb.upperBound.x, y_mid);
	}
	virtual b2Vec2 getVelocity()
	{
		return m_segments[int(numSegments/2)]->getLinearVelocity();
	}


	float totalHeight;
	float total_x;
	float getFitness() // override
	{
		float distance = getAABB().upperBound.x + length;
		//distance = distance < 0 ? 0 : distance;
		float speed = 100.f * distance / (lifeTime + 1);
		float ave_height = totalHeight / (lifeTime + 1);
		float fitness = distance; //(1.f + distance * distance) * (1.f + speed); // * (1.f + ave_height * 10.f);

		return total_x;
	}

	float getProgress()
	{
		//return getAABB().upperBound.x;
		return getFitness();
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
		
		total_x += getAABB().upperBound.x + length;

		for(int i = 0; i < numSegments-1; i++){
			eneryUsed += abs(m_joints[i]->getMotorTorque());
		}
		for(int i = 0; i < numSegments; i++){
			//m_segments[i]->applyLateralDamping(lateralDamping);
		}
		if(killOnStarvation && eneryUsed > startEnery){
			printf("died from starvation, metabolic rate = %f\n", startEnery / lifeTime);
			die();
			return;
		}

		if(lifeTime > 60){
			totalHeight += 1 - getAABB().upperBound.y;
		}
		//---------------------------------------------------------------------------------
		// Neural Net behaviour
		//---------------------------------------------------------------------------------


		// INPUT & NORMALIZATION

		int input_index = 0;
		if(touchSense){
			for(int i = 0; i < numSegments; i++){
				//neuralNet.setInput(i, m_segmentContact[i] ? 1.f : -1.f);
				neuralNet.interpolateInput(input_index++, m_segmentContact[i] ? 2.f : 0.f, contactGain);
			}
		}
		if(bodySense){
			for(int i = 0; i < numSegments - 1; i++){
				neuralNet.interpolateInput(input_index++, m_joints[i]->getAngle() * 1.f, 0.5f);
			}
		}
		if(orientationSense){
			b2Vec2 dv = m_segments[0]->getPosition() - m_segments[m_segments.size()-1]->getPosition();
			neuralNet.setInput(input_index++, atan2(dv.y, dv.x));
		}
		if(pulseFeedback){
			neuralNet.setInput(input_index++, sin(pulsePosition));
			neuralNet.setInput(input_index++, cos(pulsePosition));
		}

		if(directionSense){

			WormSegment &segment = *m_segments[0];
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
		if(muscleModel){

			for(int i = 0; i < numSegments - 1; i++){
				float o1 = neuralNet.getOutput(output_index++);
				float o2 = neuralNet.getOutput(output_index++);
				o1 = o1 < 0 ? 0 : o1;
				o2 = o2 < 0 ? 0 : o2;
				float c = m_joints[i]->getMaxMotorTorque();
				float t = max(o1,o2) * muscleTorque * 5.f + 5.f;
				m_joints[i]->setMaxMotorTorque(c + (t - c) * 1.0f);

				//float a = m_joints[i]->getAngle();
				c = m_joints[i]->getMotorSpeed();
				t = (o1 - o2) / m_world->timeStep * muscleSpeed * 0.1f;// * 0.1f;
				m_joints[i]->setMotorSpeed(c + (t - c));
			}

		} else {
			for(int i = 0; i < numSegments - 1; i++){
				float o = neuralNet.getOutput(output_index++) * maxJointAngle * 0.9f;
				float a = m_joints[i]->getAngle();
				m_joints[i]->setMotorSpeed((o - a) / m_world->timeStep * muscleSpeed * 0.1f);
			}
		}
		if(pulseFeedback){
			pulsePosition += neuralNet.getOutput(output_index++) * maxPulse;
		}	


		//---------------------------------------------------------------------------------
		// Death conditions
		//---------------------------------------------------------------------------------

		
		// Kill it off if it doesn't make progress for some time
		if(killOnLackOffProgress){
			if(getProgress() > progressPosition + progressAmount){
				progressDelay = 0;
				progressPosition = getProgress();
			} else {
				if(++progressDelay > progressTimeout){
					die();
					printf("no progress death, progress = %f, needed = %f \n", getProgress(), progressPosition);
					return;
				}
			}
		}
		if(lifeTime > maxLifeTime && maxLifeTime > 0){
			die();
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
	float eneryUsed;

	void onBeginContact(sContactPair contactPair)
	{
		for(int i = 0; i < numSegments; i++){
			if(contactPair.contains(m_segments[i])){
				//contactPair.contactInto->
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


// Body Shape
int Worm::numSegments = 7;
float Worm::length = 5.f;
//Worm:://thickness = 0.35f;
float Worm::thickness = 0.21f;
float Worm::maxJointAngle = 1.4f;

// Movement Abilities
float Worm::muscleTorque = 20;
float Worm::muscleSpeed = 0.05f;
float Worm::maxPulse = 0.2f;
float Worm::lateralDamping = 0;
bool Worm::muscleModel = true;

// Senses
bool Worm::pulseFeedback = false;
bool Worm::directFeedback = false;
bool Worm::directionSense = false;
bool Worm::bodySense = true;
bool Worm::touchSense = true;
bool Worm::orientationSense = true;
float Worm::contactGain = 0.1f;

// Death Conditions
bool Worm::killOnStarvation = false;
float Worm::startEnery = 3000;
float Worm::baseMetabolism = 0.2;
bool Worm::killOnLackOffProgress = true;
int Worm::progressTimeout = 3600;
int Worm::maxLifeTime = 0;
float Worm::progressAmount = 1;