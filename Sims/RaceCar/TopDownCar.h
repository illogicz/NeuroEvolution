#pragma once
#include "..\..\sPhysics\sWorld.h"
#include "..\..\sPhysics\sRectangle.h"
#include "..\..\sPhysics\sConvexPolygon.h"
#include "..\..\sPhysics\sRevoluteJoint.h"
#include "..\..\sEvolution\sPhenotype.h"
#include "..\..\sGraphics\sDebugDraw.h"

class TopDownCar : public sPhenotype, public sContactListener
{

public:
	TopDownCar()
	{
		width = 1;
		height = 2;
		maxEngineForce = 200;
		damping = 1.5;
		grip = 3;
		slideDamping = 0.5;
		maxStearingAngle = 1;
		visionResolution = 15;
		visionFOV = b2_pi;
		minRayLength = 1;
		maxRayLength = 30;
		touchSense = true;
		neuralFeedback = true;
		id = id_counter++;

	}

	int visionResolution;
	float visionFOV;
	float maxEngineForce;
	float maxStearingAngle;
	float width;
	float height;
	float damping;
	float grip;
	float slideDamping;
	float topSpeed;
	bool touchSense;
	bool neuralFeedback;
	b2Vec2 position;

	virtual void init(sWorld &world)
	{
		chassisFilter.categoryBits = 0x01;
		chassisFilter.maskBits = 0x01;
		//chassisFilter.groupIndex = -1 - id;

		wheelFilter.categoryBits = 0x02;
		wheelFilter.maskBits = 0x03;


		chassis.setSize(width,height);
		chassis.setFilter(chassisFilter);
		chassis.setLinearDamping(damping);
		chassis.setAngularDamping(damping);

		frontLeftWheel.copy(chassis);
		frontLeftWheel.setSize(0.3, 0.7);
		frontLeftWheel.setFilter(wheelFilter);
		//frontRightWheel.setSize(0.3, 0.7);
		//frontRightWheel.setFilter(wheelFilter);

		frontRightWheel.copy(frontLeftWheel);
		rearRightWheel.copy(frontLeftWheel);
		rearLeftWheel.copy(frontLeftWheel);


		resetPositions();

		frontLeftJoint.setAnchor(frontLeftWheel.getPosition());
		frontLeftJoint.setBodies(&chassis, &frontLeftWheel);
		frontLeftJoint.setEnableLimit(true);
		frontLeftJoint.setLimits(-0.6, 0.6);

		frontRightJoint.setAnchor(frontRightWheel.getPosition());
		frontRightJoint.setBodies(&chassis, &frontRightWheel);
		frontRightJoint.setEnableLimit(true);
		frontRightJoint.setLimits(-0.6, 0.6);

		rearLeftJoint.setAnchor(rearLeftWheel.getPosition());
		rearLeftJoint.setBodies(&chassis, &rearLeftWheel);
		rearLeftJoint.setEnableLimit(true);
		rearLeftJoint.setLimits(0, 0);

		rearRightJoint.setAnchor(rearRightWheel.getPosition());
		rearRightJoint.setBodies(&chassis, &rearRightWheel);
		rearRightJoint.setEnableLimit(true);
		rearRightJoint.setLimits(0, 0);


		add(&chassis);
		add(&frontLeftWheel);
		add(&frontRightWheel);
		add(&rearLeftWheel);
		add(&rearRightWheel);
	
		ignoreList.insert(&chassis);
		ignoreList.insert(&frontLeftWheel);
		ignoreList.insert(&frontRightWheel);
		ignoreList.insert(&rearLeftWheel);
		ignoreList.insert(&rearRightWheel);

		add(&frontLeftJoint);
		add(&frontRightJoint);
		add(&rearLeftJoint);
		add(&rearRightJoint);

		world.addContactListener(this, &chassis);
		world.addContactListener(this, &frontLeftWheel);
		world.addContactListener(this, &frontRightWheel);
		world.addContactListener(this, &rearLeftWheel);
		world.addContactListener(this, &rearRightWheel);


		int input_count = visionResolution + 1;
		if(touchSense){
			input_count++;
		}
		if(neuralFeedback){
			input_count += 1;
		}

		if(true){

			neuralNet.setLayerCount(3);
			neuralNet.setNeuronLayer(0, input_count, true, false);
			neuralNet.setNeuronLayer(1,8, true, true);
			//neuralNet.setLayerSize(2,input_count - 6);
			neuralNet.setNeuronLayer(2, 2, true, false);
			neuralNet.addSynapseLayer(0,2);

			neuralNet.setMaxWeight(3);
			neuralNet.setInitialMaxWeight(1);
			neuralNet.setConnectionsPerNeuron(30);
			neuralNet.setWeightExponent(1.5);
			

		} else {
			/*
			neuralNet.setHiddenLayerCount(1);
			neuralNet.setHiddenLayerSize(0,6);


			neuralNet.setUseFeedback(0,true);
			neuralNet.setMaxFeedback(1);
			neuralNet.setFeedbackDistribution(0,12);

			neuralNet.setMaxWeight(2);
			neuralNet.setWeightDistribution(0,4);
			neuralNet.setWeightDistribution(1,3);

			neuralNet.setMaxBias(1);
			neuralNet.setBiasDistribution(1,12);
			neuralNet.setBiasDistribution(1,12);
			neuralNet.setBiasDistribution(2,12);
			*/

		}
		
	}

	virtual void build(sWorld &world)
	{

		chassis.setType(DYNAMIC_BODY);
		frontLeftWheel.setType(DYNAMIC_BODY);
		frontRightWheel.setType(DYNAMIC_BODY);
		rearRightWheel.setType(DYNAMIC_BODY);
		rearLeftWheel.setType(DYNAMIC_BODY);

		frontLeftJoint.setLimits(0, 0);
		frontRightJoint.setLimits(0, 0);


		resetPositions();


		deferDeath = false;
		totalAccelerator = 0;
		totalSpeed = 0;
		fitnessModifier = 1;
		num_contacts = 0;
		fitnessScore = 0;

		neuralNet.update();

		if(isElite()){
			setCustomColor(b2Color(0,1,1));
		} else {
			setCustomColor(b2Color(0,0,0));
		}

	}
	virtual void destroy(sWorld &world)
	{
		chassis.setType(STATIC_BODY);
		frontLeftWheel.setType(STATIC_BODY);
		frontRightWheel.setType(STATIC_BODY);
		rearRightWheel.setType(STATIC_BODY);
		rearLeftWheel.setType(STATIC_BODY);

		//chassis.setFilter(deathFilter);
		//frontLeftWheel.setFilter(deathFilter);
		//frontRightWheel.setFilter(deathFilter);
		//rearRightWheel.setFilter(deathFilter);
		//rearLeftWheel.setFilter(deathFilter);

	}
	virtual void step()
	{

		if(deferDeath){
			die();
			return;
		}
		if(lifeTime > 6000){
			die();
		}



		applyWheelPhysics(frontLeftWheel);
		applyWheelPhysics(frontRightWheel);
		applyWheelPhysics(rearRightWheel);
		applyWheelPhysics(rearLeftWheel);
		float forwardSpeed = -b2Dot(chassis.getLinearVelocity(), b2Rot(chassis.getAngle()).GetYAxis());

		int input_index = 0;
		for(int i = 0; i <= visionResolution; i++){

			  float a;
			 if(i < visionResolution){
				 a = float(i) / (visionResolution - 1) - 0.5f;
				 a *= visionFOV;
				 a += chassis.getAngle();
				 a += b2_pi;
			 } else {
				 a = chassis.getAngle();
			 }
			 b2Vec2 p1 = b2Rot(a).GetYAxis();
			 b2Vec2 p2(p1);
			 p1 *= minRayLength;
			 p2 *= maxRayLength;
			 p1 += chassis.getPosition();
			 p2 += chassis.getPosition();
			 b2Vec2 ip;
			 sRayCastOutput result = m_world->rayCastClosest(p1,p2,&ignoreList);
			 if(result.found){
				 ip = result.point;
			 } else {
				 ip = p2;
			 }
			 if(isFocus()){
				 int c = 200 * (1.f - result.fraction) + 55;
				 m_world->getDebugDraw()->addLine(p1,ip, sf::Color(255,255,255,c));
			 }
			 neuralNet.setInput(input_index++, result.fraction * 2 - 1.f);
		}
		if(touchSense){
			neuralNet.setInput(input_index++, num_contacts ? 1 : 0);
		}

		if(neuralFeedback){
			//neuralNet.interpolateInput(input_index++, neuralNet.getOutput(0), 0.5);
			neuralNet.setInput(input_index++, forwardSpeed * 0.02);
		}

		neuralNet.run();

		float steering_output = neuralNet.getOutput(0);

		setStearing(steering_output);
		setAccelerator(neuralNet.getOutput(1));


		if(num_contacts){
			fitnessModifier *= 0.995;
			setCustomColor(b2Color(1,0,0));
		} else {
			if(isElite()){
				setCustomColor(b2Color(0,1,1));
			} else {
				setCustomColor(b2Color(0,0,0));
			}
		}

		//if(forwardSpeed > 0){
			fitnessScore += forwardSpeed * (1 - abs(steering_output));
		//}// else {
		//	//fitnessScore += forwardSpeed;
		//}

		//fitnessScore += chassis.getLinearVelocity().Length();

	}
	virtual b2Vec2 getPosition()
	{
		return chassis.getPosition();
	}
	virtual b2Vec2 getVelocity()
	{
		return chassis.getLinearVelocity();
	}
	virtual float getFitness()
	{
		//if(totalSpeed > 0){
		//	return totalSpeed * fitnessModifier;
		//} else {
		//	return 0;
		//}
		
	//	if(fitnessScore < 1){
	//		fitnessScore = 1;
	//	}
		//return fitnessScore * fitnessModifier;
		float score = fitnessScore;
		if(score < 1){
			score = 1;
		}
		return score * fitnessModifier;

	};	


	sRectangle chassis;
	sRectangle frontLeftWheel;
	sRectangle frontRightWheel;
	sRectangle rearLeftWheel;
	sRectangle rearRightWheel;
	
	sRevoluteJoint frontLeftJoint;
	sRevoluteJoint frontRightJoint;
	sRevoluteJoint rearLeftJoint;
	sRevoluteJoint rearRightJoint;

	float minRayLength;
	float maxRayLength;

	float fitnessScore;

private:

	void resetPositions()
	{
		chassis.zeroState();
		frontLeftWheel.zeroState();
		frontRightWheel.zeroState();
		rearRightWheel.zeroState();
		rearLeftWheel.zeroState();

		b2Vec2 p = position + b2Vec2(sRandom::getFloat(-5, 5), sRandom::getFloat(-2, 2));

		chassis.setPosition(p);
		frontLeftWheel.setPosition(p.x - width, p.y - height);
		frontRightWheel.setPosition(p.x + width, p.y - height);
		rearRightWheel.setPosition(p.x - width, p.y + height);
		rearLeftWheel.setPosition(p.x + width, p.y + height);

		frontLeftJoint.setAnchor(frontLeftWheel.getPosition());
		frontRightJoint.setAnchor(frontRightWheel.getPosition());
		rearLeftJoint.setAnchor(rearLeftWheel.getPosition());
		rearRightJoint.setAnchor(rearRightWheel.getPosition());

	}

	static int id_counter;
	int id;
	bool deferDeath;
	b2Filter chassisFilter;
	b2Filter wheelFilter;
	float totalAccelerator;
	float totalSpeed;
	float fitnessModifier;
	int num_contacts;
	set<sBody*> ignoreList;
	void setAccelerator(float acc)
	{
		//totalAccelerator -= acc;// > 0 ? 0 : acc;
		b2Rot a(rearLeftWheel.getAngle());
		rearLeftWheel.applyForceToCenter(maxEngineForce * acc * a.GetYAxis());
		a.Set(rearRightWheel.getAngle());
		rearRightWheel.applyForceToCenter(maxEngineForce * acc * a.GetYAxis());
	}

	void setStearing(float stearing)
	{
		float a = stearing * maxStearingAngle;
		float c = frontLeftJoint.getUpperLimit();
		float v = c + (a - c) * 0.05;
		frontLeftJoint.setLimits(v,v);
		frontRightJoint.setLimits(v,v);
	}



	sBodyState initialState;

	void applyWheelPhysics(sBody &body)
	{
		b2Vec2 v = b2Mul(b2Rot(-body.getAngle()), body.getLinearVelocity());

		float t = 1.0;
		if(v.x > grip) t = slideDamping;
		v.x *= 1.f - t;

		body.setLinearVelocity(b2Mul(b2Rot(body.getAngle()), v));
		
	}

	void onBeginContact(sContactPair contactPair)
	{
		num_contacts++;
	}
	void onEndContact(sContactPair contactPair)
	{
		num_contacts--;
	}
};

int TopDownCar::id_counter = 0;