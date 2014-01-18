#pragma once
#include "../sPhysics/sWorld.h";
#include "sPopulation.h"

class sSimulation
{
	sSimulation()
	{
		init();
	}

	virtual void init()
	{
		// create phenotypes
		resetSimulation();
		simFrame = 0;
	}

	virtual void resetSimulation()
	{
		for(int i = 0; i < population.size(); i++){
			world.add(population[i]);
			world.remove(population[i]);
		}
	}

	void step()
	{
		simFrame++;
		if(isFinished()){
			population.newGeneration();
			resetSimulation();
			simFrame = 0;
		}
		world.step();


		float bestFitness = -100;
		for(int i=0;i <population.size(); i++){
			float fitness = population[i]->getFitness();
			if(fitness > bestFitness){
				bestFitness = fitness;
				leader = population[i];
			}
		}
	}


	virtual bool isFinished()
	{
		for(int i = 0; i < population.size(); i++){
			if(population[i]->alive) return false;
		}
		return true;
	}


	sPhenotype *leader;
	sPhenotype *elite;
	int simFrame;
	sPopulation population;
	sWorld world;

};