#pragma once
#include "..\..\sEvolution\sPhenotype.h"
#include "..\..\sPhysics\sRectangle.h"
#include "..\..\sPhysics\sRevoluteJoint.h"




class PoleCart : public sPhenotype, public sContactListener
{
public:


	virtual void init(sWorld &world)
	{

		b2Filter filter;
		filter.groupIndex = -1;
		pole1Height = 5;
		pole2Height = b2_pi / 2;
		velocityInfo = false;

		cart.setPosition(0,-1);
		cart.setFriction(0);
		cart.setSize(3,1);
		cart.setFilter(filter);

		pole1.setLinearDamping(0);
		pole1.setAngularDamping(0);
		pole1.setFilter(filter);
		joint1.setBodies(&cart, &pole1);

		pole2.copy(pole1);
		pole2.setFilter(filter);
		joint2.setBodies(&cart, &pole2);


		add(&cart);

		add(&pole1);
		add(&joint1);

		add(&pole2);
		add(&joint2);


		world.addContactListener(this, &pole1);
		world.addContactListener(this, &pole2);


		genome.setUseMutationRateGene(true, 0, 0.007f);

		neuralNet.setLayerCount(3);
		neuralNet.setNeuronLayer(0,velocityInfo ? 7 : 3, true, true);
		neuralNet.setNeuronLayer(1,5, true, true);
		neuralNet.setNeuronLayer(2,1, true, true);
		//neuralNet.setNeuronLayer(3,1, true, true);
		neuralNet.addSynapseLayer(0,2);	
		//neuralNet.addSynapseLayer(0,3);	
		//neuralNet.addSynapseLayer(0,3);	
		neuralNet.setWeightExponent(2);	
		neuralNet.setMaxWeight(3);
		neuralNet.setInitialMaxWeight(2);

	}

	bool velocityInfo;
	float pole1Height;
	float pole2Height;

	b2Vec2 position;

	virtual void build(sWorld &world)
	{
		cart.setType(KINEMATIC_BODY);
		pole1.setType(DYNAMIC_BODY);
		pole2.setType(DYNAMIC_BODY);
		//pole3.setType(DYNAMIC_BODY);

		cart.zeroState();
		pole1.zeroState();
		pole2.zeroState();
		//pole3.zeroState();

		pole1.setSize(0.5,pole1Height);
		pole2.setSize(0.5,pole2Height);
		//pole3.setSize(0.3,pole3Height);



		cart.setPosition(position.x,position.y - 1);

		pole1.setPosition(position.x - 1, position.y - 1 - pole1Height);
		joint1.setAnchor(position.x - 1, position.y - 1);

		pole2.setPosition(position.x + 1, position.y - 1 - pole2Height);
		joint2.setAnchor(position.x + 1, position.y - 1);

		//pole3.setPosition(position.x + 0.5, position.y - 1 - pole3Height - pole1Height * 2);
		//joint3.setAnchor(position.x + 0.5, position.y - 1  - pole1Height * 2);

		float unbalance = 0.1f;

		float a1 = sRandom::getFloat(-unbalance, unbalance);
		float a2 = sRandom::getFloat(-unbalance, unbalance);

		b2Vec2 p = pole1.getPosition();
		p -= joint1.getAnchor();
		p = b2Mul(b2Rot(a1),p);
		p += joint1.getAnchor();
		pole1.setPosition(p);
		pole1.setAngle(a1);

		p = pole2.getPosition();
		p -= joint2.getAnchor();
		p = b2Mul(b2Rot(a2),p);
		p += joint2.getAnchor();
		pole2.setPosition(p);
		pole2.setAngle(a2);


		//cart.applyForceToCenter(b2Vec2(sRandom::getFloat(-1000,1000), 0));

		neuralNet.update();

		fitnessModifier = 1;
		deferDeath = false;
		totalForce = 0;
		rnd = sRandom::getFloat(0,1);	
	}
	virtual void destroy(sWorld &world)
	{
		cart.setType(STATIC_BODY);
		pole1.setType(STATIC_BODY);
		pole2.setType(STATIC_BODY);
	
	}
	virtual void step() 
	{
		if(deferDeath)return die();
		
		float a1 = pole1.getAngle();
		float a2 = pole2.getAngle();
		
		if(abs(a1) > b2_pi * 0.9)return die();
		if(abs(a2) > b2_pi * 0.9)return die();

		int index = 0;
		neuralNet.setInput(index++,a1);
		neuralNet.setInput(index++,a2);
		float x = cart.getPosition().x - position.x;
		if(abs(x) > 10){
			if(x > 0){
				x -= 10;
			} else {
				x += 10;
			}
			x /= 100;
		} else {
			x = 0;
		}
		neuralNet.setInput(index++,x);
		if(velocityInfo){
			//neuralNet.setInput(index++,cart.getLinearVelocity().x * 0.05);
			neuralNet.setInput(index++, pole1.getAngularVelocity() * 0.1f);
			neuralNet.setInput(index++, pole2.getAngularVelocity() * 0.1f);

		}


		neuralNet.run();
		//cart.applyForceToCenter(b2Vec2(neuralNet.getOutput(0) * 1000,0));
		float xv = cart.getLinearVelocity().x;
		cart.setLinearVelocity(b2Vec2(neuralNet.getOutput(0) * 40 + xv, 0));

		totalForce += abs(neuralNet.getOutput(0));

		if(lifeTime >= 10000){
			//fitnessModifier =  (20000 - totalForce) / 10000;
			die();
		}

	}
	virtual b2Vec2 getPosition()
	{
		return cart.getPosition();
	}
	virtual b2Vec2 getVelocity()
	{
		return cart.getLinearVelocity();
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
	float rnd;
	float fitnessModifier;
	float totalForce;
	bool deferDeath;
	sRectangle cart;
	sRectangle pole1;
	sRectangle pole2;

	sRevoluteJoint joint1;
	sRevoluteJoint joint2;

};

