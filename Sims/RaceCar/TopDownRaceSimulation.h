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

		ground.setSize(120,65);
		ground.setPosition(0,3);

		setGravity(0,0);
		world.setGroundBody(&ground);
		world.add(&ground);

		simulationTime = 100000;

		staticView = true;
		renderScale = 10;

		populationSize = 20;
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

protected:

	void initPhenotypes()
	{

		for(int i = 0; i < populationSize; i++){
			TopDownCar *car = new TopDownCar;
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



	}



};