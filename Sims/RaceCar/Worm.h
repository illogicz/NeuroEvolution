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
		length = 5.f;
		thickness = 0.7f;
		numSegments = 7;
	}

	~Worm()
	{
		for(int i = 0; i < numSegments; i++){
			delete m_segments[i];
		}
		for(int i = 0; i < numSegments-1; i++){
			delete m_joints[i];
		}
	}

	int numSegments;

	float length;
	float thickness;

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
			world.addContactListener(this, m_segments[i]);
			add(m_segments[i]);

		}

		m_joints.resize(numSegments-1);
		for(int i = 0; i < numSegments-1; i++){
			sRevoluteJoint* joint = m_joints[i] = new sRevoluteJoint;
			joint->setBodies(m_segments[i], m_segments[i+1]);
			joint->setEnableLimit(true);
			joint->setEnableMotor(true);
			joint->setMaxMotorTorque(1000.f);
			joint->setLimits(-1.5f, 1.5f);
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
		neuralNet.setHiddenLayerSize(0,numSegments+3);
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
			
			float h1 = float(sin(float(i) / numSegments * b2_pi) * thickness * 0.4f + 0.1f);
			float h2 = float(sin(float(i + 1) / numSegments * b2_pi) * thickness * 0.4 + 0.1f);
			float w = length / numSegments * 0.5f;
			float x = float(i) / numSegments * length;

			segment->zeroState();
			segment->setPosition(x, 0);

			segment->clearVerices();
			segment->addVertex(-w, h1);
			segment->addVertex( w, h2);
			segment->addVertex( w,-h2);
			segment->addVertex(-w,-h1);

		}

		for(int i = 0; i < numSegments-1; i++){
			sRevoluteJoint* joint = m_joints[i];
			float x = float(i + 0.5f) / numSegments * length;
			joint->setAnchor(x,0);
			joint->setMotorSpeed(0);
		}

		// Initialise some values for simulation
		neuralNet.prepare();
		fitnessModifier = 1;
		progressPosition = getPosition().x;
		progressDelay = 0;
		deferDeath = false;
		beatPosition = 0.f;

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
	
	virtual float getFitness() 
	{
		float distance = getPosition().x;
		//if(distance < 0)fitnessModifier = 0;
		float speed = abs(distance) / float(lifeTime + 1) + 0.1f;
		//return float(pow(2, fitnessModifier * distance * speed * 0.1f)); //payloadMass;// / wheelMass;
		return fitnessModifier * distance * speed; //payloadMass;// / wheelMass;
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
			neuralNet.setInput(i, m_segmentContact[i] ? 1.f : -1.f);
		}
		neuralNet.setInput(numSegments, sin(beatPosition) * 3.f);
		neuralNet.setInput(numSegments+1, cos(beatPosition) * 3.f);
		//

	
		// Run neural net ////////////////////////////////
		neuralNet.run();
		//////////////////////////////////////////////////


		// OUTPUT
		for(int i = 0; i < numSegments - 1; i++){
			float o = neuralNet.getOutput(i) * 1.0f;
			float a = m_joints[i]->getAngle();
			m_joints[i]->setMotorSpeed((o - a) * 0.99f);
			//m_joints[i]->setMotorSpeed(o * 0.5f);
		}
		beatPosition += neuralNet.getOutput(numSegments - 1) * 0.3f;


		//---------------------------------------------------------------------------------
		// Death conditions
		//---------------------------------------------------------------------------------

		
		// Kill it off if it doesn't make progress for some time
		if(getPosition().x > progressPosition){
			progressDelay = 0;
			progressPosition = getPosition().x + 0.01f;
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