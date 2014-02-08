#pragma once
#include "..\..\sPhysics\sWorld.h"
#include "..\..\sPhysics\sRectangle.h"
#include "..\..\sPhysics\sConvexPolygon.h"
#include "..\..\sPhysics\sRevoluteJoint.h"
#include "..\..\sEvolution\sPhenotype.h"
#include "..\..\sGraphics\sDebugDraw.h"
#include "RaceTrack.h"

class TopDownCar : public sPhenotype, public sContactListener
{

public:
	TopDownCar()
	{
		width = 1;
		height = 2;
		maxEngineForce = 400;
		damping = 0.5;
		grip = 1;
		slideDamping = 0.2;
		maxStearingAngle = 0.75;
		visionResolution = 7;
		visionFOV = b2_pi;
		minRayLength = 1;
		maxRayLength = 50;
		maxLifeTime = 1800;
		touchSense = true;
		neuralFeedback = true;
		reverseTrack = false;
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
		chassisFilter.categoryBits = 0x02;
		chassisFilter.maskBits = 0x01;
		//chassisFilter.groupIndex = -1 - id;

		wheelFilter.categoryBits = 0x03;
		wheelFilter.maskBits = 0x04;


		chassis.setSize(width,height);
		chassis.setFilter(chassisFilter);
		chassis.setLinearDamping(damping);
		chassis.setAngularDamping(damping);
		chassis.setFriction(5);

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
	
		whiteList.insert(&track->innerWall);
		whiteList.insert(&track->outerWall);

		//ignoreList.insert(&chassis);
		//ignoreList.insert(&frontLeftWheel);
		//ignoreList.insert(&frontRightWheel);
		//ignoreList.insert(&rearLeftWheel);
		//ignoreList.insert(&rearRightWheel);

		add(&frontLeftJoint);
		add(&frontRightJoint);
		add(&rearLeftJoint);
		add(&rearRightJoint);

		world.addContactListener(this, &chassis);
		world.addContactListener(this, &frontLeftWheel);
		world.addContactListener(this, &frontRightWheel);
		world.addContactListener(this, &rearLeftWheel);
		world.addContactListener(this, &rearRightWheel);


		int input_count = visionResolution;
		if(touchSense){
			input_count++;
		}
		if(neuralFeedback){
			input_count += 4;
		}

		if(true){

			neuralNet.setLayerCount(4);
			neuralNet.setNeuronLayer(0, input_count, false, false);
			neuralNet.setNeuronLayer(1, 8, true, true);
			neuralNet.setNeuronLayer(2, 4, true, true);
			neuralNet.setNeuronLayer(3, 2, true, true);
			//neuralNet.setNeuronLayer(3, 2, true, true);
			neuralNet.addSynapseLayer(0,2);
			neuralNet.addSynapseLayer(1,3);
			//neuralNet.addSynapseLayer(0,3);
			//neuralNet.addSynapseLayer(1,3);

			neuralNet.setMaxWeight(2);
			neuralNet.setWeightExponent(2);
			

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

		
		


		deferDeath = false;
		totalAccelerator = 0;
		totalSpeed = 0;
		totalStearing = 0;
		fitnessModifier = 1;
		num_contacts = 0;
		fitnessScore = 0.3;
		last_angle = 0;
		progressPotition = getFitness();
		progressTime = 0;
		reverseTrack = !reverseTrack;

		resetPositions();
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
		if(lifeTime > maxLifeTime){
			die();
		}



		applyWheelPhysics(frontLeftWheel);
		applyWheelPhysics(frontRightWheel);
		applyWheelPhysics(rearRightWheel);
		applyWheelPhysics(rearLeftWheel);
		float forwardSpeed = -b2Dot(chassis.getLinearVelocity(), b2Rot(chassis.getAngle()).GetYAxis());
		float sidewaysSpeed = -b2Dot(chassis.getLinearVelocity(), b2Rot(chassis.getAngle()).GetXAxis());

		// TODO : TOI inputs instead of distance

		b2Vec2 chassisPos = chassis.getPosition();

		int input_index = 0;
		for(int i = 0; i < visionResolution; i++){

			float f = float(i) / (visionResolution - 1) - 0.5f;
			 float a;
			 if(i < visionResolution){
				 a = f;
				 a *= visionFOV;
				 //a += chassis.getAngle();
				 a += b2_pi;
			 } else {
				 a = chassis.getAngle();
			 }
			 b2Vec2 p1 = b2Rot(a).GetYAxis();
			 b2Vec2 p2(p1);
			 p1 *= minRayLength;
			 p2 *= maxRayLength;

			 p2.x *= 0.5;
			 p2.y *= 1 - abs(f);

			 p1 = b2Mul(b2Rot(chassis.getAngle()), p1);
			 p2 = b2Mul(b2Rot(chassis.getAngle()), p2);

			 p1 += chassisPos;
			 p2 += chassisPos;
			 b2Vec2 ip;
			// sRayCastOutput result = m_world->rayCastClosest(p1,p2,&ignoreList);
			 sRayCastOutput result = m_world->rayCastClosest(p1,p2, &whiteList, false);
			 if(result.found){
				 ip = result.point;
			 } else {
				 ip = p2;
			 }
			 if(isFocus()){
				 int c = 200 * (1.f - result.fraction) + 55;
				 m_world->getDebugDraw()->addLine(p1,ip, sf::Color(255,255,255,c));
			 }
			 neuralNet.setInput(input_index++, 1.f - result.fraction);
		}
		if(touchSense){
			neuralNet.setInput(input_index++, num_contacts ? 1 : 0);
		}

		float progressAngle = atan2f(chassisPos.y, chassisPos.x);

		b2Vec2 target;
		float lookAhead = 0.1;
		while(true){
			b2Vec2 p = track->getTrackPoint(chassisPos, reverseTrack ? -lookAhead : lookAhead);
			sRayCastOutput result = m_world->rayCastClosest(p, chassisPos, &whiteList, false);
			target = p;
			if(result.found || lookAhead > 0.45) break;
			lookAhead += 0.05;
		}
		if(neuralFeedback){

			b2Vec2 td = target - chassisPos;
			float ta = atan2f(td.y, td.x);

			float a = ta - chassis.getAngle() + b2_pi / 2;
			while(a > b2_pi) a -= b2_pi * 2;
			while(a < -b2_pi) a += b2_pi * 2;



			//if(isFocus()){
			//	m_world->getDebugDraw()->addLine(trackPos, chassis.getPosition(), sf::Color(255,100,255));
			//}


			neuralNet.interpolateInput(input_index, a, 0.3f);

			if(isFocus()){
				b2Vec2 p2 = chassisPos;
				p2.x += 30 * cos(neuralNet.getInput(input_index) + chassis.getAngle() - b2_pi / 2);
				p2.y += 30 * sin(neuralNet.getInput(input_index) + chassis.getAngle() - b2_pi / 2);

				m_world->getDebugDraw()->addLine(p2, chassisPos, sf::Color(255,100,255, 55));
			}


			input_index++;
			neuralNet.interpolateInput(input_index++, chassis.getAngularVelocity() * 0.1f, 0.5f);
			neuralNet.interpolateInput(input_index++, forwardSpeed * 0.02f, 0.5f);
			neuralNet.interpolateInput(input_index++, sidewaysSpeed * 0.03f, 0.5f);



		}




		neuralNet.run();

		float steering_output = neuralNet.getOutput(0);

		setStearing(steering_output);
		setAccelerator(neuralNet.getOutput(1));


		if(num_contacts){
			fitnessModifier *= 0.99;
			if(fitnessModifier < 0.5f){
				setCustomColor(b2Color(0,0,0));
				return die();
			}
			setCustomColor(b2Color(1,0,0));
		} else {
			if(isElite()){
				setCustomColor(b2Color(0,1,1));
			} else {
				setCustomColor(b2Color(0,0,0));
			}
		}

		//if(forwardSpeed > 0){


		
		progressSpeed = last_angle - progressAngle;
		if(progressSpeed > b2_pi) progressSpeed -= b2_pi * 2;
		if(progressSpeed < -b2_pi) progressSpeed += b2_pi * 2;

		fitnessScore += reverseTrack ? -progressSpeed : progressSpeed;
		totalStearing += abs(steering_output);	

		last_angle = progressAngle;

		//}// else {
		//	//fitnessScore += forwardSpeed;
		//}

		//fitnessScore += chassis.getLinearVelocity().Length();

		if(getFitness() > progressPotition + 0.1){
			progressTime = 0;
			progressPotition = getFitness();
		} else {
			if(progressTime++ > 300){
				die();
			}
		}

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
		//float score = fitnessScore;
		//if(score < 1){
		//	score = 1;
		//}
		float sm = 1.2f - (totalStearing / (lifeTime + 1));
		return fitnessScore * fitnessModifier * sm; //score * fitnessModifier;

	};	

	float progressSpeed;

	sRectangle chassis;
	sRectangle frontLeftWheel;
	sRectangle frontRightWheel;
	sRectangle rearLeftWheel;
	sRectangle rearRightWheel;
	
	sRevoluteJoint frontLeftJoint;
	sRevoluteJoint frontRightJoint;
	sRevoluteJoint rearLeftJoint;
	sRevoluteJoint rearRightJoint;

	int maxLifeTime;
	
	bool reverseTrack;

	float minRayLength;
	float maxRayLength;

	float fitnessScore;
	float totalStearing;
	RaceTrack *track;
	float progressPotition;
	int progressTime;
private:
	float last_angle;
	void resetPositions()
	{
		chassis.zeroState();
		frontLeftWheel.zeroState();
		frontRightWheel.zeroState();
		rearRightWheel.zeroState();
		rearLeftWheel.zeroState();

		float a = reverseTrack ? b2_pi : 0;

		b2Transform t(position, b2Rot(a));

		//b2Vec2 p = position;// + b2Vec2(sRandom::getFloat(-1, 1), sRandom::getFloat(-1, 1));

		chassis.setPosition(position);
		chassis.setAngle(a);

		//frontLeftWheel.setPosition(b2Mul(t, p.x - width, p.y - height);
		frontLeftWheel.setPosition( b2Mul(t, b2Vec2(-width, -height)));
		frontLeftWheel.setAngle(a);
		frontRightWheel.setPosition(b2Mul(t, b2Vec2( width, -height)));
		frontRightWheel.setAngle(a);
		rearRightWheel.setPosition( b2Mul(t, b2Vec2(-width,  height)));
		rearRightWheel.setAngle(a);
		rearLeftWheel.setPosition(  b2Mul(t, b2Vec2( width,  height)));
		rearLeftWheel.setAngle(a);

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
	set<sBody*> whiteList;

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
		if(abs(v.x) > grip) t = slideDamping;
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