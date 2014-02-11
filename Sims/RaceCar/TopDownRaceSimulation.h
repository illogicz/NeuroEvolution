#pragma once
#include "../../sEvolution/sSimulation.h"
#include "TopDownCar.h"
#include "RaceTrack.h"
#include "../../sPhysics/sEdgeRectangle.h"
#include "../../sPhysics/sCircle.h"
#include "../../sUtils/perlin.h"
/*

*/

class TopDownRaceSimulation : public sSimulation
{


public:

	TopDownRaceSimulation()
	{

		width = 150;
		height = 80;

		ground.radius = 90;
		ground.maxOffset = 10;
		ground.minWidth = 23;
		ground.maxWidth = 35;
		//ground.generateTrack();
		//world.add(&ground);
		population.setWinnersPerPrelim(8);

		gravity.Set(0,0);
		
		c.setType(STATIC_BODY);
		worlds[0].add(&c);
		worlds[0].setGroundBody(&c);

		//simulationTime = 100000;

		breadingPoolFraction = 0.7;
		selectionBias = 1.7;

		staticView = true;
		renderScale = 3.3;
		zoomScale = 2.4;
		populationSize = 40;

	}

	~TopDownRaceSimulation()
	{
		for(int i = 0; i < populationSize; i++){
			delete population[i];
		}
	}
	
	int populationSize;
	//float simulationTime;
	
	float width;
	float height;
protected:

	void initPhenotypes()
	{

		int w = ceil(sqrt(populationSize)) + 1;
		int h = floor(float(populationSize) / w);

		for(int i = 0; i < populationSize; i++){
			TopDownCar *car = new TopDownCar;
			//car->position.x = float((i % w) + 1) / (w+1) * width - width / 2;
			//car->position.y = float((i / w) + 1) / (h+1) * height - height / 2;
			car->position.x = ground.radius;
			car->position.y = 0;
			car->track = &ground;
			car->startPosition = float(i) / populationSize * b2_pi * 2;

			addPhenotype(car);
		}

	}


	bool isFinished()
	{

		for(int i = 0; i < population.size(); i++){
			if(population[i]->alive) return false;
		}

		return true;
	}

	// Build / Rebuild environment
	// Builds the ground based on roughness inputs
	void buildEnvironment()
	{
		
		if(ground.maxOffset < 60 && population.prelimsComplete())ground.maxOffset += 0.5;
		ground.generateTrack();
		for(int i = 0; i < worlds.size(); i++){
			worlds[0].remove(&ground);
			worlds[0].add(&ground);
		}
			//ground.setPosition(0,0);


	}
	RaceTrack ground;
	sCircle c;
};