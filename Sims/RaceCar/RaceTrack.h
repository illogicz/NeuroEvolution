#pragma once
#include "..\..\sPhysics\sContainer.h"
#include "..\..\sPhysics\sChain.h"
#include "..\..\sUtils\perlin.h"


class RaceTrack : public sContainer
{
public:


	RaceTrack()
	{
		radius = 50;
		maxOffset = 20;
		minWidth = 5;
		maxWidth = 15;
		numSegments = 100;
		innerWall.makeLoop(true);
		outerWall.makeLoop(true);
		innerWall.setFriction(2);
		outerWall.setFriction(2);
		add(&innerWall);
		add(&outerWall);
	}




	void generateTrack()
	{


		Perlin perlin1(1,1,1,sRandom::getInt(0,100000));
		Perlin perlin2(1,1,1,sRandom::getInt(0,100000));

		innerWall.clear();
		outerWall.clear();
		trackPoints.clear();



		for(int i = 0; i < numSegments; i++){

			float a = float(i) / numSegments * b2_pi * 2;
			float r = radius + perlin1.Get(float(i) / numSegments * 10, 0) * maxOffset;	

			float w = (perlin2.Get(float(i) / numSegments * 10, 0) + 1) * (maxWidth - minWidth) + minWidth * 0.5f;

			b2Vec2 p(cos(a) * r, sin(a) * r);
			trackPoints.push_back(p);
			innerWall.addVertex(cos(a) * (r + w), sin(a) * (r + w));
			outerWall.addVertex(cos(a) * (r - w), sin(a) * (r - w));

		}

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

	sChain innerWall;
	sChain outerWall;
	int numSegments;
	float radius;
	float maxOffset;
	float minWidth;
	float maxWidth;


};