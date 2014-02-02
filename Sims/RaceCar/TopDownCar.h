#pragma once
#include "..\..\sPhysics\sWorld.h"
#include "..\..\sPhysics\sRectangle.h"
#include "..\..\sPhysics\sConvexPolygon.h"
#include "..\..\sPhysics\sRevoluteJoint.h"
#include "..\..\sEvolution\sPhenotype.h"
#include "..\..\sGraphics\sDebugDraw.h"

class sTopDownRaceFitness : public sFitnessFunction
{
public: 
	 float operator()(sPhenotype *phenotype)
	{
		return 1;
	}
};

class TopDownCar : public sPhenotype, public sContactListener
{

public:
	TopDownCar()
	{
		width = 1;
		height = 2;
		maxEngineForce = 50;
		maxStearingAngle = 1;
		visionResolution = 9;
		visionFOV = 2.5;
		minRayLength = 1;
		maxRayLength = 30;
		fitnessFunction = &raceFitness;
		id = id_counter++;
		position.x = -50 + id * 5;
		position.y = 20;
	}

	int visionResolution;
	float visionFOV;
	float maxEngineForce;
	float maxStearingAngle;
	float width;
	float height;
	b2Vec2 position;

	virtual void init(sWorld &world)
	{
		lifeFilter.categoryBits = 0x01;
		lifeFilter.maskBits = 0x01;
		lifeFilter.groupIndex = -1 - id;
		deathFilter.groupIndex = -1 - id;
		deathFilter.categoryBits = 0x02;
		deathFilter.maskBits = 0x02;


		chassis.setSize(width,height);
		chassis.setFilter(lifeFilter);
		chassis.setLinearDamping(0.5);
		frontLeftWheel.setSize(0.3, 0.7);
		frontLeftWheel.setFilter(lifeFilter);
		frontRightWheel.setSize(0.3, 0.7);
		frontRightWheel.setFilter(lifeFilter);

		//frontRightWheel.copy(frontLeftWheel);
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


		neuralNet.setInputCount(visionResolution);
		neuralNet.setHiddenLayerCount(2);
		neuralNet.setHiddenLayerSize(0,visionResolution + 2);
		neuralNet.setHiddenLayerSize(1,5);


		neuralNet.setUseFeedback(0,true);
		neuralNet.setFeedbackDistribution(0,3);

		neuralNet.setMaxWeight(2.6);
		neuralNet.setWeightDistribution(0,7);
		neuralNet.setWeightDistribution(1,8);
		neuralNet.setWeightDistribution(2,4);

		neuralNet.setMaxBias(1);
		neuralNet.setBiasDistribution(1,3);
		neuralNet.setBiasDistribution(2,5);

		neuralNet.setOutputCount(2);
		neuralNet.create(genome);

	}

	virtual void build(sWorld &world)
	{

		chassis.setType(DYNAMIC_BODY);
		frontLeftWheel.setType(DYNAMIC_BODY);
		frontRightWheel.setType(DYNAMIC_BODY);
		rearRightWheel.setType(DYNAMIC_BODY);
		rearLeftWheel.setType(DYNAMIC_BODY);

		chassis.setFilter(lifeFilter);
		frontLeftWheel.setFilter(lifeFilter);
		frontRightWheel.setFilter(lifeFilter);
		rearRightWheel.setFilter(lifeFilter);
		rearLeftWheel.setFilter(lifeFilter);

		frontLeftJoint.setLimits(0, 0);
		frontRightJoint.setLimits(0, 0);

		resetPositions();

		deferDeath = false;
		totalAccelerator = 0;
		totalSpeed = 0;
		fitnessModifier = 1;
		num_contacts = 0;

		neuralNet.prepare();

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

		chassis.setFilter(deathFilter);
		frontLeftWheel.setFilter(deathFilter);
		frontRightWheel.setFilter(deathFilter);
		rearRightWheel.setFilter(deathFilter);
		rearLeftWheel.setFilter(deathFilter);

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

		if(num_contacts){
			fitnessModifier *= 0.999;
			setCustomColor(b2Color(1,0,0));
		} else {
			if(isElite()){
				setCustomColor(b2Color(0,1,1));
			} else {
				setCustomColor(b2Color(0,0,0));
			}
		}

		totalSpeed += chassis.getLinearVelocity().Length();

		applyWheelPhysics(frontLeftWheel);
		applyWheelPhysics(frontRightWheel);
		applyWheelPhysics(rearRightWheel);
		applyWheelPhysics(rearLeftWheel);

		int input_index = 0;
		for(int i = 0; i < visionResolution; i++){
			 float a = float(i) / (visionResolution - 1) - 0.5f;
			 a *= visionFOV;
			 a += chassis.getAngle();
			 a += b2_pi;

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
			 if(isLeader()){
				 int c = 200 * (1.f - result.fraction) + 55;
				 m_world->getDebugDraw()->addLine(p1,ip, sf::Color(255,255,255,c));
			 }
			 neuralNet.setInput(input_index++, 1.f - result.fraction);
		}

		neuralNet.run();

		setStearing(neuralNet.getOutput(0));
		setAccelerator(neuralNet.getOutput(1));


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
	    return fitnessModifier * totalSpeed; //(*fitnessFunction)(this);		
	};	


	sTopDownRaceFitness raceFitness;

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



private:

	void resetPositions()
	{
		chassis.zeroState();
		frontLeftWheel.zeroState();
		frontRightWheel.zeroState();
		rearRightWheel.zeroState();
		rearLeftWheel.zeroState();

		chassis.setPosition(position);
		frontLeftWheel.setPosition(position.x - width, position.y - height);
		frontRightWheel.setPosition(position.x + width, position.y - height);
		rearRightWheel.setPosition(position.x - width, position.y + height);
		rearLeftWheel.setPosition(position.x + width, position.y + height);
	}

	static int id_counter;
	int id;
	bool deferDeath;
	b2Filter lifeFilter;
	b2Filter deathFilter;
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
		if(v.x > 5) t = 0.5;
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