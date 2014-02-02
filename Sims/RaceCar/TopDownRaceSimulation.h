#pragma once
#include "../../sEvolution/sSimulation.h"
#include "TopDownCar.h"
#include "../../sPhysics/sEdgeRectangle.h"
#include "../../sPhysics/sCircle.h"
#include "../../sUtils/perlin.h"
/*

*/

class TopDownRaceSimulation : public sSimulation
{
private:

	class RaceFitness : public sFitnessFunction
	{
	public: 
		 b2Vec2 target;
		 float32 distance;
		 float operator()(sPhenotype *phenotype)
		{
			float distance_traveled = phenotype->getPosition().Length();
			float distance_target = (phenotype->getPosition() - target).Length();
			return distance - distance_target + distance_traveled * 0.5;
			//return distance_death - distance_target;
		}
	};
	sEdgeRectangle ground;

public:

	TopDownRaceSimulation()
	{

		width = 150;
		height = 80;

		ground.setSize(width,height);
		ground.setPosition(0,0);

		setGravity(0,0);
		world.setGroundBody(&ground);
		world.add(&ground);

		simulationTime = 100000;

		staticView = true;
		renderScale = 8;

		populationSize = 50;
	}

	~TopDownRaceSimulation()
	{
		for(int i = 0; i < populationSize; i++){
			delete population[i];
		}
	}
	
	int populationSize;
	float simulationTime;
	RaceFitness fitnessFunction;
	float width;
	float height;
protected:

	void initPhenotypes()
	{

		int w = sqrt(populationSize);
		int h = ceil(float(populationSize) / w);

		for(int i = 0; i < populationSize; i++){
			TopDownCar *car = new TopDownCar;
			car->position.x = float((i % w) + 1) / (w+1) * width - width / 2;
			car->position.y = float((i / w) + 1) / (h+1) * height - height / 2;
			car->init(world);
			population.addPhenotype(car);
			world.add(car);
			if(!i){
				car->neuralNet.printStats();
				car->genome.printStats();
			}
		}
		population.printStats();

	}


	// Step fucntion
	// Return whether the simulation is finished
	// In this case it's when all phenotypes are no longer active/alive
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

		ground.setSize(width,height);
		ground.setPosition(0,0);


	}



};