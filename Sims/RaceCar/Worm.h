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
		numSegments = 7;
		length = 5.f;
		thickness = 0.35f;
		maxJointAngle = 1.3f;
		muscleTorque = 10.1f;
		muscleSpeed = 0.1f;
		maxPulse = 0.2f;
		contactGain = 0.1f;
		
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

	int numSegments;
	float length;
	float thickness;
	float maxJointAngle;
	float muscleTorque;
	float muscleSpeed;
	float maxPulse;
	float contactGain;

protected:

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

		genome.addGene("scale", 0.7, 1.3);

		//---------------------------------------------------------------------------------
		// Genome Definitions
		//---------------------------------------------------------------------------------

		
//		genome.addGene("num_segments", 0.7, 1.3);
				
		

		//---------------------------------------------------------------------------------
		// Neural Network Definition
		//---------------------------------------------------------------------------------
 
		neuralNet.setInputCount(numSegments+2);
		neuralNet.setOutputCount(numSegments);
		neuralNet.setHiddenLayerCount(1);
		//neuralNet.setHiddenLayerSize(0,numSegments+3);
		neuralNet.setHiddenLayerSize(0,4);
		neuralNet.setMaxBias(1);
		neuralNet.setMaxWeight(1);
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

			float s = sin(maxJointAngle);
			float c = cos(maxJointAngle);

			segment->clearVerices();
			segment->addVertex(-w, h1);
			segment->addVertex( w, h2);
			segment->addVertex( w + h2 * s, h2 * c);
			segment->addVertex( w + h2 * s, -h2 * c);
			segment->addVertex( w,-h2);
			segment->addVertex(-w,-h1);
			segment->addVertex(-w - h1 * s, -h1 * c);
			segment->addVertex(-w - h1 * s, h1 * c);

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
		beatPosition = 0.f;
		maxHeight =  0.f;
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
	
	float maxHeight;

	virtual float getFitness() 
	{
		if(!isInWorld()) return 0;
		b2AABB aabb = getAABB();
		//if(isLeader()) printf("%f, %f   %f, %f \n", aabb.lowerBound.x, aabb.lowerBound.y, aabb.upperBound.x, aabb.upperBound.y);

		float distance = getPosition().x;

		float height = -(aabb.lowerBound.y);
		if(height > maxHeight) maxHeight = height;
		//if(isLeader())printf("%f \n", distance);
		//if(distance < 0)fitnessModifier = 0;
		float speed = abs(distance) / float(lifeTime + 1) + 0.001f;
		//return float(pow(2, fitnessModifier * distance * speed * 0.1f)); //payloadMass;// / wheelMass;
		return fitnessModifier * distance * speed; //payloadMass;// / wheelMass;
		//return maxHeight;
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

		int index = 0;
		for(int i = 0; i < numSegments; i++){
			//neuralNet.setInput(i, m_segmentContact[i] ? 1.f : -1.f);
			neuralNet.interpolateInput(i, m_segmentContact[i] ? 1.f : -1.f, contactGain);
		}
		neuralNet.setInput(numSegments, sin(beatPosition));
		neuralNet.setInput(numSegments+1, cos(beatPosition));
		//

	
		// Run neural net ////////////////////////////////
		neuralNet.run();
		//////////////////////////////////////////////////


		// OUTPUT
		for(int i = 0; i < numSegments - 1; i++){
			float o = neuralNet.getOutput(i) * maxJointAngle;
			float a = m_joints[i]->getAngle();
			m_joints[i]->setMotorSpeed((o - a) / m_world->timeStep * muscleSpeed);
			//m_joints[i]->setMotorSpeed(o * 0.5f);
		}
		beatPosition += neuralNet.getOutput(numSegments - 1) * maxPulse;


		//---------------------------------------------------------------------------------
		// Death conditions
		//---------------------------------------------------------------------------------

		
		// Kill it off if it doesn't make progress for some time
		if(getFitness() > progressPosition + 0.001f){
			progressDelay = 0;
			progressPosition = getFitness();
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

	
	static const int progressTimeout = 600;
	int progressDelay;
	float progressPosition;
	float fitnessModifier;
	bool deferDeath;
	float beatPosition;

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