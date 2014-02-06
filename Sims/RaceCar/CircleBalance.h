#pragma once

#include "..\..\sPhysics\sWorld.h"
#include "..\..\sEvolution\sPhenotype.h"
#include "..\..\sPhysics\sCircle.h"




class CircleBalance : public sPhenotype, public sContactListener
{
public:

	CircleBalance()
	{
		id = id_counter++;
	}

	virtual void init(sWorld &world)
	{

		b2Filter filter;
		filter.categoryBits = 0x02;
		filter.maskBits = 0x01;
		filter.groupIndex = id + 1;

		radii.push_back(1.4);
		radii.push_back(1.4);
		radii.push_back(1.4);
		radii.push_back(1.4);
		num_circles = radii.size();

		circles.resize(num_circles);

		for(int i = 0; i < num_circles; i++){
			circles[i].setRadius(radii[i]);
			circles[i].setFriction(10);
			circles[i].setLinearDamping(0);
			circles[i].setAngularDamping(0);
			circles[i].setFilter(filter);
			if(i){
				world.addContactListener(this, &circles[i], groundBody);
			}
			add(&circles[i]);
		}

		genome.setUseMutationRateGene(true, 0,0.01);

		neuralNet.setLayerCount(4);
		neuralNet.setNeuronLayer(0,num_circles * 2 - 1, true, true);
		neuralNet.setNeuronLayer(1,5, true, true);
		neuralNet.setNeuronLayer(2,3, true, true);
		neuralNet.setNeuronLayer(3,2, true, true);
		//neuralNet.setNeuronLayer(3,1, true, true);
		neuralNet.addSynapseLayer(0,2);	
		neuralNet.addSynapseLayer(0,3);	
		neuralNet.addSynapseLayer(1,3);	
		//neuralNet.addSynapseLayer(0,3);	
		//neuralNet.addSynapseLayer(0,3);	
		neuralNet.setWeightExponent(1.5);	
		neuralNet.setConnectionsPerNeuron(100);	
		neuralNet.setMaxWeight(4);
		neuralNet.setInitialMaxWeight(1);

	}

	bool velocityInfo;
	vector<float> radii;
	vector<sCircle> circles;
	int num_circles;
	sBody *groundBody;
	b2Vec2 position;

	virtual void build(sWorld &world)
	{

		for(int i = 0; i < num_circles; i++){
			circles[i].zeroState();

			if(i){
				circles[i].setType(DYNAMIC_BODY);
				float a = sRandom::getFloat(-0.1, 0.1);
				b2Vec2 dv(0,radii[i-1] + radii[i]);
				dv = b2Mul(b2Rot(a), dv);
				circles[i].setPosition(circles[i-1].getPosition() - dv);

			} else {			
				circles[i].setType(KINEMATIC_BODY);
				circles[i].setPosition(position.x, position.y - radii[i]);
			}
		}


		neuralNet.update();

		fitnessModifier = 1;
		deferDeath = false;
		totalForce = 0;
		rnd = sRandom::getFloat(0,1);	


	}
	virtual void destroy(sWorld &world)
	{
		for(int i = 0; i < num_circles; i++){
			circles[i].setType(STATIC_BODY);
		}	
		setCustomColor(b2Color(0,0,0));
	}

	virtual void step() 
	{
		if(deferDeath)return die();
		
		int input_index = 0;
		for(int i = 0; i < num_circles - 1; i++){
			if(circles[i+1].getPosition().y > circles[i].getPosition().y - radii[i] ){
				//return die();
			}
			b2Vec2 dv = circles[i].getPosition() - circles[i+1].getPosition();
			neuralNet.setInput(input_index++, atan2f(dv.x, dv.y));
		}
		neuralNet.setInput(input_index++, (circles[0].getPosition().x - position.x) * 0.02);
		for(int i = 1; i < num_circles; i++){
			neuralNet.setInput(input_index++, circles[i].getAngularVelocity() * 0.1);
		}
		neuralNet.run();

		float av = circles[0].getAngularVelocity() + neuralNet.getOutput(0);
		circles[0].setAngularVelocity(av);
		float lv = circles[0].getLinearVelocity().x + neuralNet.getOutput(1);
		circles[0].setLinearVelocity(b2Vec2(lv, 0));

		if(lifeTime >= 10000){
			//fitnessModifier =  (20000 - totalForce) / 10000;
			die();
		}

		if(isFocus()){
			setCustomColor(b2Color(0,1,1));
		} else {
			setCustomColor(b2Color(0,0,0));
		}

	}
	virtual b2Vec2 getPosition()
	{
		return circles[0].getPosition();
	}
	virtual b2Vec2 getVelocity()
	{
		return circles[0].getLinearVelocity();
	}

	virtual float getFitness()
	{
		return lifeTime * fitnessModifier + rnd;
	}	


	void onBeginContact(sContactPair contactPair)
	{
		deferDeath = true;
	}



private:
	int id;
	static int id_counter;
	float rnd;
	float fitnessModifier;
	float totalForce;
	bool deferDeath;
	
};

int CircleBalance::id_counter = 0;