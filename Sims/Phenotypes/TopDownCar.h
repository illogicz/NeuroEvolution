#pragma once
#include "..\..\sPhysics\sWorld.h"
#include "..\..\sPhysics\sRectangle.h"
#include "..\..\sPhysics\sConvexPolygon.h"
#include "..\..\sPhysics\sRevoluteJoint.h"
#include "..\..\sEvolution\sPhenotype.h"
#include "..\..\sGraphics\sDebugDraw.h"
#include "..\Environments\RaceTrack.h"

class TopDownCar : public sPhenotype, public sContactListener
{

public:
	TopDownCar()
	{
		width = 1;
		height = 2;
		maxEngineForce = 330;
		damping = 2;
		grip = 0.5;
		slideDamping = 0.3;
		maxStearingAngle = 0.7;
		visionResolution = 11;
		visionFOV = b2_pi;
		minRayLength = 1;
		maxRayLength = 40;
		maxLifeTime = 1800;
		reverseTrack = false;
		collideWithOthers = true;
		id = id_counter++;

	}

	bool collideWithOthers;
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
	b2Vec2 position;

	virtual void init(sWorld &world)
	{
		if (collideWithOthers){
			chassisFilter.groupIndex =  -1 - id;
			wheelFilter.groupIndex =  -1 - id;
		}
		else {
			chassisFilter.categoryBits = 0x02;
			chassisFilter.maskBits = 0x01;
			wheelFilter.categoryBits = 0x02;
			wheelFilter.maskBits = 0x01;
		}

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


	
		whiteList.insert(track);
		//whiteList.insert(&track->outerWall);

		blackList.insert(&chassis);
		blackList.insert(&frontLeftWheel);
		blackList.insert(&frontRightWheel);
		blackList.insert(&rearLeftWheel);
		blackList.insert(&rearRightWheel);

		add(&frontLeftJoint);
		add(&frontRightJoint);
		add(&rearLeftJoint);
		add(&rearRightJoint);

		add(&chassis);
		add(&frontLeftWheel);
		add(&frontRightWheel);
		add(&rearLeftWheel);
		add(&rearRightWheel);

		world.addContactListener(this, &chassis);
		world.addContactListener(this, &frontLeftWheel);
		world.addContactListener(this, &frontRightWheel);
		world.addContactListener(this, &rearLeftWheel);
		world.addContactListener(this, &rearRightWheel);


		int input_count = visionResolution + 9;
		// 1 reverse ray
		// 2, 3, 4 velocities
		// 5, 6 touch
		// 7 direction
		// 8, 9, feedback

		if(true){

			neuralNet.setLayerCount(4);
			neuralNet.setNeuronLayer(0, input_count, false, false);
			neuralNet.setNeuronLayer(1, 6, true, true);
			neuralNet.setNeuronLayer(2, 4, true, true);
			neuralNet.setNeuronLayer(3, 2, true, true);
			//neuralNet.setNeuronLayer(3, 2, true, true);
			neuralNet.addSynapseLayer(0,2);
			neuralNet.addSynapseLayer(2,1);
			//neuralNet.addSynapseLayer(0,3);
			neuralNet.addSynapseLayer(1,3);

			neuralNet.setMaxWeight(2);
			neuralNet.setWeightExponent(1.f);
			

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
		fitnessScore = 0.0;
		progressPotition = getFitness();
		progressTime = 0;
		reverseTrack = !reverseTrack;
		frontLeftContact = false;
		frontRightContact = false;
		rearLeftContact = false;
		rearRightContact = false;

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

		//remove(&chassis);
		//remove(&frontLeftWheel);
		//remove(&frontRightWheel);
		//remove(&rearLeftWheel);
		//remove(&rearRightWheel);

	}
	virtual void step()
	{

		if(deferDeath){
			return die();
		}
		if(lifeTime > maxLifeTime){
			return die();
		}



		applyWheelPhysics(frontLeftWheel);
		applyWheelPhysics(frontRightWheel);
		applyWheelPhysics(rearRightWheel);
		applyWheelPhysics(rearLeftWheel);

		//----------------------------------------------------------------------------------------
		// Set ANN inputs
		//-----------------------------------------------------------------------------------------

		b2Vec2 chassisPos = chassis.getPosition();
		int input_index = 0;

		// nueral feedback
		neuralNet.interpolateInput(input_index++, neuralNet.getOutput(0), 0.2f);
		neuralNet.interpolateInput(input_index++, neuralNet.getOutput(1), 0.2f);


		// RAY CASTS
		for(int i = 0; i <= visionResolution; i++){

			float f = float(i) / (visionResolution - 1) - 0.5f;
			float a;
			if(i < visionResolution){
				a = f;
				a *= visionFOV;
				a += b2_pi;
			} else {
				a = 0;
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
			sRayCastOutput result = collideWithOthers ? 
				m_world->rayCastClosest(p1, p2, &blackList, true) :
				m_world->rayCastClosest(p1, p2, &whiteList, false);

			if(result.found){
				ip = result.point;
				if(result.body != NULL){
					b2Vec2 v = result.body->GetLinearVelocityFromWorldPoint(ip);

					b2Vec2 dp = p2 - p1;
					float dv = b2Dot(dp, v);
					float l = dp.Normalize();

					float m = (dv / l) * 0.05;
					//result.fraction = result.fraction + (dv / l) * 0.5f;
					result.fraction -= 0.1;
					result.fraction *= 1 + m;
					result.fraction += 0.1;

					if(result.fraction < 0)result.fraction = 0;
					if(result.fraction > 1)result.fraction = 1;

					dp = (result.fraction * l) * dp;

					if(isFocus()){
						//m_world->getDebugDraw()->addLine(ip,p1 + dp, sf::Color(100,255,200,255));
						m_world->getDebugDraw()->addLine(ip,ip + v, sf::Color(100,0,200,255));
					}
					ip = p1 + dp;
					
				}
				 


			 } else {
				 ip = p2;
			 }
			 
			 if(isFocus()){
				 int c = 200 * (1.f - result.fraction) + 55;
				 m_world->getDebugDraw()->addLine(p1,ip, sf::Color(255,255,255,c));
			 }
			 neuralNet.setInput(input_index++, 1.f - result.fraction);
		}

		// TOUCH

		// left / right
		bool left = rearLeftContact || frontLeftContact;
		bool right = rearRightContact || frontRightContact;
		float v = 0;
		if(left && !right) v = 1;
		if(!left && right) v = -1;
		if(v == 0){
			neuralNet.interpolateInput(input_index++, 0, 0.1);
		} else {
			neuralNet.setInput(input_index++, v);
		}
		// front / back
		bool rear = rearLeftContact || rearRightContact;
		bool front = frontLeftContact || frontRightContact;
		v = 0;
		if(rear && !front) v = 1;
		if(!rear && front) v = -1;
		if(v == 0){
			neuralNet.interpolateInput(input_index++, 0, 0.1);
		} else {
			neuralNet.setInput(input_index++, v);
		}

		float progressAngle = atan2f(chassisPos.y, chassisPos.x);

		// DIRECTION
		b2Vec2 target;
		float lookAhead = 0.1;
		int n = 0;
		while(true){
			b2Vec2 p = track->getTrackPoint(chassisPos, reverseTrack ? -lookAhead : lookAhead);
			sRayCastOutput result = m_world->rayCastClosest(p, chassisPos, &whiteList, false);
			if(result.found || lookAhead > 1.2){
				if(!n)target = p;
				break;
			}
			target = p;
			lookAhead += 0.05;
			n++;
		}
		b2Vec2 td = target - chassisPos;
		float ta = atan2f(td.y, td.x);

		float a = ta - chassis.getAngle() + b2_pi / 2;
		while(a > b2_pi) a -= b2_pi * 2;
		while(a < -b2_pi) a += b2_pi * 2;
		neuralNet.interpolateInput(input_index, a / b2_pi, 0.3f);

		if(isFocus()){
			b2Vec2 p2 = chassisPos;
			p2.x += 30 * cos(neuralNet.getInput(input_index) * b2_pi + chassis.getAngle() - b2_pi / 2);
			p2.y += 30 * sin(neuralNet.getInput(input_index) * b2_pi + chassis.getAngle() - b2_pi / 2);

			m_world->getDebugDraw()->addLine(p2, chassisPos, sf::Color(255,100,255, 55));
		}	
		input_index++;

		// VELOCITIES
		b2Rot ba(chassis.getAngle());
		float forwardSpeed = -b2Dot(chassis.getLinearVelocity(), ba.GetYAxis());
		float sidewaysSpeed = -b2Dot(chassis.getLinearVelocity(), ba.GetXAxis());

		neuralNet.interpolateInput(input_index++, chassis.getAngularVelocity() * 0.1f, 0.5f);
		neuralNet.interpolateInput(input_index++, forwardSpeed * 0.02f, 0.5f);
		neuralNet.interpolateInput(input_index++, sidewaysSpeed * 0.03f, 0.5f);



		//----------------------------------------------------------------------------------------
		// Run Neural Net
		//-----------------------------------------------------------------------------------------
		neuralNet.run();


		//----------------------------------------------------------------------------------------
		// Use Outputs
		//-----------------------------------------------------------------------------------------


		float steering_output = neuralNet.getOutput(0);

		setStearing(steering_output);
		setAccelerator(neuralNet.getOutput(1));


		if(num_contacts){
			//if(fitnessModifier > 0.05){
			//	fitnessModifier *= 0.999;
			//}
			fitnessScore -= 0.001;
			//if(fitnessModifier < 0.5f){
			//	setCustomColor(b2Color(0,0,0));
			//	return die();
			//}
			setCustomColor(b2Color(1,0,0));
		} else {
			if(isElite()){
				setCustomColor(b2Color(0,1,1));
			} else {
				setCustomColor(b2Color(0,0,0));
			}
		}

		
		progressSpeed = last_angle - progressAngle;
		if(progressSpeed > b2_pi) progressSpeed -= b2_pi * 2;
		if(progressSpeed < -b2_pi) progressSpeed += b2_pi * 2;

		fitnessScore += reverseTrack ? -progressSpeed : progressSpeed;
		totalStearing += abs(steering_output);	

		last_angle = progressAngle;

		

		//fitnessScore += chassis.getLinearVelocity().Length();
		/*
		if(getFitness() > progressPotition + 0.1){
			progressTime = 0;
			progressPotition = getFitness();
		} else {
			if(progressTime++ > 1000){
				die();
			}
		}
		*/
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
		//float sm = 1.f - (totalStearing / (lifeTime + 1));
		//sm *= fitnessModifier;
		//if(fitnessScore > 0){
		//	return fitnessScore * sm; //score * fitnessModifier;
		//} else {
			return fitnessScore;
		//}
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
	float startPosition;

	float minRayLength;
	float maxRayLength;

	float fitnessScore;
	float totalStearing;
	RaceTrack *track;
	float progressPotition;
	int progressTime;
private:
	


	void resetPositions()
	{
		chassis.zeroState();
		frontLeftWheel.zeroState();
		frontRightWheel.zeroState();
		rearRightWheel.zeroState();
		rearLeftWheel.zeroState();

		//float r = track->radius + sRandom::getFloat(-10, 10);
		float px = sin(startPosition) * track->radius;
		float py = cos(startPosition) * track->radius;

		position = track->getTrackPoint(b2Vec2(px, py), 0);
		float r = position.Normalize();
		position *= r + sRandom::getFloat(-7, 7);

		b2Vec2 p2 = track->getTrackPoint(position, reverseTrack ? - 0.4 : 0.4);
		p2 -= position;
		float a = atan2f(p2.y, p2.x) + b2_pi/2;

		//if(reverseTrack) a += b2_pi;
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

		last_angle = atan2f(chassis.getPosition().y, chassis.getPosition().x);

	}

	static int id_counter;
	int id;
	bool deferDeath;
	b2Filter chassisFilter;
	b2Filter wheelFilter;
	//b2Filter deathFilter;
	float totalAccelerator;
	float totalSpeed;
	float fitnessModifier;
	int num_contacts;
	float last_angle;

	bool frontLeftContact;
	bool frontRightContact;
	bool rearLeftContact;
	bool rearRightContact;

	set<sObject*> whiteList;
	set<sObject*> blackList;

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

		if(contactPair.contains(&frontLeftWheel)){
			frontLeftContact = true;
		} else if(contactPair.contains(&frontRightWheel)){
			frontRightContact = true;
		} else if(contactPair.contains(&rearLeftWheel)){
			rearLeftContact = true;
		} else if(contactPair.contains(&rearRightWheel)){
			rearRightContact = true;
		}
		num_contacts++;
	}
	void onEndContact(sContactPair contactPair)
	{
		if(contactPair.contains(&frontLeftWheel)){
			frontLeftContact = false;
		} else if(contactPair.contains(&frontRightWheel)){
			frontRightContact = false;
		} else if(contactPair.contains(&rearLeftWheel)){
			rearLeftContact = false;
		} else if(contactPair.contains(&rearRightWheel)){
			rearRightContact = false;
		}
		num_contacts--;
	}
};

int TopDownCar::id_counter = 0;