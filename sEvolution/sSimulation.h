#pragma once

#include "..\sPhysics\sWorld.h"
#include "sPopulation.h"

class sSimulation
{
	friend class sSimulationDisplay;
	friend class sSimulationWindow;

public:
	
	sSimulation()
	{
		world.setGravity(b2Vec2(0,10));
		elites = 1;
		
		mutationRate = 0.01f;
		selectionBias = 1.5f;
		breadingPoolFraction = 1.0;
		renderScale = 25;
		speedUp = false;
		staticView = false;
	}

	sPopulation population;

	int elites;
	float mutationRate;
	float selectionBias;
	float breadingPoolFraction;
	bool speedUp;
	float renderScale;
	bool staticView;

	sPhenotype *leader;
	sPhenotype *liveLeader;
	sPhenotype *trailer;
	sPhenotype *elite;

	void setGravity(float x, float y)
	{
		world.setGravity(b2Vec2(x, y));
	}

protected:

	
	void init()
	{
		
		initPhenotypes();
		resetSimulation();
		population.setElites(elites);
		population.setSelectionBias(selectionBias);
		population.setMutationRate(mutationRate);
		population.setBreadingPoolFraction(breadingPoolFraction);
		renderCenter.Set(0,0);
		simFrame = 0;
	}

	void resetSimulation()
	{
		for(unsigned int i = 0; i < population.size(); i++){
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
		calcRankings();
		return newGen;
	}

	void calcRankings()
	{

		population.sortPhenotypes();
		sPhenotype *lastLeader = leader;
		leader = population[0];
		//leader->setIsLeader(false);
		for(int i = 0; i < population.size(); i++){
			population[i]->setRank(i);
		}
		int i = population.size();
		while(i--){
			if(population[i]->alive){
				trailer = population[i];
				break;
			}
		}
		if(leader != lastLeader){
			if(lastLeader != nullptr)
				lastLeader->setIsLeader(false);
			leader->setIsLeader(true);
		}
	}



	virtual void initPhenotypes() = 0;
	virtual void buildEnvironment() = 0;
	virtual bool isFinished() = 0;



	int simFrame;
	b2Vec2 renderCenter;
	sWorld world;

};