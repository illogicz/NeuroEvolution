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
		ground.maxOffset = 20;
		ground.minWidth = 10;
		ground.maxWidth = 25;
		//ground.generateTrack();
		//world.add(&ground);
		population.setWinnersPerPrelim(1);

		setGravity(0,0);
		world.setGroundBody(&ground.innerWall);

		//simulationTime = 100000;

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
		world.remove(&ground);
		if(ground.maxOffset < 75)ground.maxOffset += 0.7;
		ground.generateTrack();
		world.add(&ground);
		//ground.setPosition(0,0);


	}
	RaceTrack ground;
	//sEdgeRectangle ground;

};