#pragma once

#include "../sPhysics/sWorld.h"
#include "sPopulation.h"

class sSimulation
{
	friend class sSimulationDisplay;
	friend class sSimulationWindow;

public:
	
	sSimulation()
	{
		elites = 1;
		mutationRate = 0.01f;
		selectionBias = 1.5f;
		breadingPoolFraction = 1.0;
		speedUp = false;
	}

	int elites;
	float mutationRate;
	float selectionBias;
	float breadingPoolFraction;
	bool speedUp;

	sPhenotype *leader;
	sPhenotype *liveLeader;
	sPhenotype *trailer;
	sPhenotype *elite;

protected:

	
	void init()
	{
		world.setGravity(b2Vec2(0,10));
		initPhenotypes();
		resetSimulation();
		population.setElites(elites);
		population.setSelectionBias(selectionBias);
		population.setMutationRate(mutationRate);
		population.setBreadingPoolFraction(breadingPoolFraction);
		renderScale = 25;
		renderCenter.Set(0,0);
		simFrame = 0;
	}

	void resetSimulation()
	{
		for(int i = 0; i < population.size(); i++){
			if(i < population.getElites() && population.getGenerationCount()){
				//population[i]->setCustomColor(b2Color(0,0.5,0.9));
			} else {
				//population[i]->setCustomColor(b2Color(0,0,0));
			}
			world.remove(population[i]);
			world.add(population[i]);
		}
		buildEnvironment();
		leader = nullptr;
	}


	bool step()
	{
		bool newGen = false;
		simFrame++;
		if(isFinished()){
			population.newGeneration();
			elite = population[0];
			resetSimulation();
			simFrame = 0;
			newGen = true;
		}

		world.step();

		return newGen;
	}

	b2Vec2 getFocus(bool showTrailer)
	{
		float bestFitness = -100;
		float bestLiveFitness = -100;
		float worstFitness = 1000000.f;
		for(int i=0;i <population.size(); i++){
			float fitness = population[i]->getFitness();
			if(fitness > bestLiveFitness && population[i]->alive){
				bestLiveFitness = fitness;
				liveLeader = population[i];
			}
			if(fitness > bestFitness){
				bestFitness = fitness;
				leader = population[i];
			}
			if(population[i]->alive){
				if(fitness < worstFitness){
					worstFitness = fitness;
					trailer = population[i];
				}
			}
		}
		return showTrailer ? trailer->getPosition() : leader->getPosition();
	}


	virtual void initPhenotypes() = 0;
	virtual void buildEnvironment() = 0;
	virtual bool isFinished() = 0;



	int simFrame;
	float renderScale;
	b2Vec2 renderCenter;
	sPopulation population;
	sWorld world;

};