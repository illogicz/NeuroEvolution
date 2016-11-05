#pragma once
#include "..\..\sPhysics\sContainer.h"
#include "..\..\sPhysics\sChain.h"
#include "..\..\sUtils\perlin.h"


class RaceTrack : public sObject
{
public:


	RaceTrack() : sObject(CUSTOM_OBJECT)
	{
		radius = 50;
		maxOffset = 20;
		minWidth = 5;
		maxWidth = 15;
		numSegments = 100;
		
		innerWallFixture.shape = &innerWallShape;
		innerWallFixture.friction = 1.f;
		innerWallFixture.restitution = 0.1f;

		outerWallFixture.shape = &outerWallShape;
		outerWallFixture.friction = 1.f;
		outerWallFixture.restitution = 0.1f;

		bodyDef.type = b2BodyType::b2_staticBody;

		//add(&innerWall);
		//add(&outerWall);
	}




	void generateTrack()
	{


		Perlin perlin1(1,1,1,sRandom::getInt(0,100000));
		Perlin perlin2(1,1,1,sRandom::getInt(0,100000));

		innerWall.resize(numSegments);
		outerWall.resize(numSegments);
		trackPoints.resize(numSegments);



		for(int i = 0; i < numSegments; i++){

			float a = float(i) / numSegments * b2_pi * 2;
			float r = radius + perlin1.Get(float(i) / numSegments * 10, 0) * maxOffset;	

			float w = (perlin2.Get(float(i) / numSegments * 10, 0) + 1) * (maxWidth - minWidth) + minWidth * 0.5f;

			b2Vec2 p(cos(a) * r, sin(a) * r);
			trackPoints[i] = p;
			innerWall[i] = b2Vec2(cos(a) * (r + w), sin(a) * (r + w));
			outerWall[i] = b2Vec2(cos(a) * (r - w), sin(a) * (r - w));

		}
		innerWallShape.m_vertices = NULL;
		innerWallShape.m_count = 0;
		innerWallShape.CreateLoop(&innerWall[0], innerWall.size());
		outerWallShape.m_vertices = NULL;
		outerWallShape.m_count = 0;
		outerWallShape.CreateLoop(&outerWall[0], outerWall.size());
	}


	b2Vec2 getTrackPoint(b2Vec2 p, float lookAhead = 0.3)
	{
		float a = atan2f(p.y, p.x);
		float r = (a - lookAhead) / (b2_pi * 2) * numSegments;
		while(r < 0)r += numSegments;
		int i1 = int(floor(r)) % numSegments;
		int i2 = int(ceil(r)) % numSegments;

		float t = r - i1;

		
		return trackPoints[i1] + t * (trackPoints[i2] - trackPoints[i1]);
	}
	
	vector<b2Vec2> trackPoints;
	vector<b2Vec2> innerWall;
	vector<b2Vec2> outerWall;
	map<sWorld*,b2Body*> bodies;

	b2ChainShape innerWallShape;
	b2ChainShape outerWallShape;
	b2FixtureDef innerWallFixture;
	b2FixtureDef outerWallFixture;

	b2BodyDef bodyDef;

	int numSegments;
	float radius;
	float maxOffset;
	float minWidth;
	float maxWidth;

protected:
	virtual void addToWorld(sWorld &world)
	{
		b2Body* body = bodies[&world] = world.b2world.CreateBody(&bodyDef);
		body->CreateFixture(&innerWallFixture);
		body->CreateFixture(&outerWallFixture);
		body->SetUserData(this);

	} 
	virtual void removeFromWorld(sWorld &world)
	{
		world.b2world.DestroyBody(bodies[&world]);
	}

};