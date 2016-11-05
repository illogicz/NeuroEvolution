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

		radii.push_back(1);
		radii.push_back(1);
		//radii.push_back(1.4);
		num_circles = radii.size();

		circles.resize(num_circles);

		for(int i = 0; i < num_circles; i++){
			circles[i].setRadius(radii[i]);
			circles[i].setFriction(10);
			circles[i].setLinearDamping(0);
			circles[i].setAngularDamping(0);
			circles[i].setRestitution(1);
			circles[i].setFilter(filter);
			if(i){
				//world.addContactListener(this, &circles[i], groundBody);
			}
			add(&circles[i]);
		}

		genome.setUseMutationRateGene(true, 0.0005f, 0.01f);

		neuralNet.setLayerCount(3);
		neuralNet.setNeuronLayer(0,10, false, false);
		neuralNet.setNeuronLayer(1,7, false, true);
		neuralNet.setNeuronLayer(2,2, false, false);
		//neuralNet.setNeuronLayer(3,1, true, true);
		neuralNet.addSynapseLayer(0,2);	
		//neuralNet.addSynapseLayer(1,1);	
		//neuralNet.addSynapseLayer(0,3);	
		//neuralNet.addSynapseLayer(1,3);	
 		neuralNet.setWeightExponent(1);	
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
				float a = sRandom::getFloat(-0.1f, 0.1f);
				b2Vec2 dv(0,radii[i-1] + radii[i]);
				dv = b2Mul(b2Rot(a), dv);
				dv.y += 1;
				circles[i].setPosition(circles[i-1].getPosition() - dv);

			} else {			
				circles[i].setType(KINEMATIC_BODY);
				circles[i].setPosition(position.x, position.y - radii[i]);
			}
		}


		neuralNet.update();
		energyUsed = 0;
		totalDistance = 0;
		fitnessModifier = 1;
		deferDeath = false;
		totalForce = 0;
		fellOver = false;
		contact = false;
		rnd = sRandom::getFloat(0,1);	

		if(isElite()){
			genome.setValue("mutationRate", genome.getValue("mutationRate") * 0.99f);
		}
		world.addContactListener(this, &circles[0], &circles[1]);
		setAlpha(1);

	}
	virtual void destroy(sWorld &world)
	{
		for(int i = 0; i < num_circles; i++){
			circles[i].setType(STATIC_BODY);
		}	
		setCustomColor(b2Color(0,0,0));
		setAlpha(0);
	}

	virtual void step() 
	{
		if(deferDeath)return die();
		
		int input_index = 0;
		for(int i = 0; i < num_circles - 1; i++){
			if(circles[i+1].getPosition().y > circles[i].getPosition().y){
				fellOver = true;
				return die();
			}
			b2Vec2 dv = circles[i].getPosition() - circles[i+1].getPosition();
			neuralNet.setInput(input_index++, atan2f(dv.x, dv.y));

			float dist = dv.Length() - radii[i] - radii[i+1];

			neuralNet.setInput(input_index++, dist / 30.f);
		}
		neuralNet.setInput(input_index++, (circles[0].getPosition().x - position.x) * 0.05f);
		neuralNet.setInput(input_index++, (circles[0].getPosition().y - position.y) * 0.05f);
		neuralNet.setInput(input_index++, (circles[0].getLinearVelocity().x) * 0.05f);
		neuralNet.setInput(input_index++, (circles[0].getLinearVelocity().y) * 0.05f);
		neuralNet.setInput(input_index++, (circles[1].getLinearVelocity().x) * 0.05f);
		neuralNet.setInput(input_index++, (circles[1].getLinearVelocity().y) * 0.05f);
		neuralNet.setInput(input_index++, contact ? 1.f : -1.f);

		for(int i = 1; i < num_circles; i++){
			neuralNet.setInput(input_index++, circles[i].getAngularVelocity() * 0.1f);
		}
		neuralNet.run();

		float lvy = circles[0].getLinearVelocity().y + neuralNet.getOutput(0);
		float lvx = circles[0].getLinearVelocity().x + neuralNet.getOutput(1);
		circles[0].setLinearVelocity(b2Vec2(lvx, lvy));

		b2Vec2 origin_dist = circles[0].getPosition() - position;
		b2Vec2 balls_dist = circles[1].getPosition() - circles[0].getPosition();

		if(lifeTime >= 10000 || origin_dist.LengthSquared() > 500.f || balls_dist.LengthSquared() > 3000.f){
			//fitnessModifier =  (20000 - totalForce) / 10000;
			
			die();
		}


		totalDistance += (circles[0].getPosition() - circles[1].getPosition()).Length() - radii[0] - radii[1];


		energyUsed += abs(neuralNet.getOutput(0)) + abs(neuralNet.getOutput(1));
		if(isElite()){
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
		float extra = 0;
		if(lifeTime > 6000){
			extra = totalDistance / lifeTime;
			//extra += abs(circles[1].getAngle()) * 0.1f;
		}
		return lifeTime * fitnessModifier + extra;
	}	


	void onBeginContact(sContactPair contactPair)
	{
		contact = true;
		//deferDeath = true;
	}
	void onEndContact(sContactPair contactPair)
	{
		contact = false;
		//deferDeath = true;
	}


private:

	bool contact;
	bool fellOver;
	float energyUsed;
	float totalDistance;
	int id;
	static int id_counter;
	float rnd;
	float fitnessModifier;
	float totalForce;
	bool deferDeath;
	
};

int CircleBalance::id_counter = 0;