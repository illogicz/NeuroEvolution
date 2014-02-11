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
		elites = 1;
		worlds.resize(1);
		mutationRate = 0.01f;
		selectionBias = 1.5f;
		breadingPoolFraction = 1.0;
		renderScale = 25;
		zoomScale = 1;
		speedUp = false;
		staticView = false;
		phenomesPerWorld = 0;
		gravity.Set(0,9.8);
		staticViewPosition.Set(0,0);
	}

	sPopulation population;

	int elites;
	float mutationRate;
	float selectionBias;
	float breadingPoolFraction;
	bool speedUp;
	float renderScale;
	float zoomScale;
	bool staticView;
	int phenomesPerWorld;
	b2Vec2 gravity;
	b2Vec2 staticViewPosition;

	sPhenotype *leader;
	sPhenotype *liveLeader;
	sPhenotype *trailer;


protected:

	
	void init()
	{
		buildEnvironment();
		initPhenotypes();
		resetSimulation();
		for(int i = 0; i < worlds.size(); i++){
			worlds[i].setGravity(gravity);
		}
		population.setElites(elites);
		population.setSelectionBias(selectionBias);
		population.setMutationRate(mutationRate);
		population.setBreadingPoolFraction(breadingPoolFraction);
		renderCenter.Set(0,0);
		simFrame = 0;
		printStats();
	}

	virtual void resetSimulation()
	{
		buildEnvironment();
		for(unsigned int i = 0; i < population.size(); i++){
			int wi = phenomesPerWorld ? i / phenomesPerWorld : 0;
			worlds[wi].remove(population[i]);
			worlds[wi].add(population[i]);
		}
		leader = nullptr;
	}


	bool step()
	{
		bool newGen = false;
		simFrame++;
		if(isFinished()){
			population.newGeneration();
			resetSimulation();
			simFrame = 0;
			newGen = true;
		}

		for(int i = 0; i < worlds.size(); i++){
			worlds[i].step();
		}

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

	void addPhenotype(sPhenotype *phenotype)
	{
		if(phenomesPerWorld){

			int i = population.size();
			if(i / phenomesPerWorld <= worlds.size()){
				worlds.resize(worlds.size() + 1);
			}
			phenotype->init(worlds[i / phenomesPerWorld]);
			worlds[i / phenomesPerWorld].add(phenotype);

		} else {

			phenotype->init(worlds[0]);
			worlds[0].add(phenotype);

		}

		population.addPhenotype(phenotype);
		
	}

	void printStats()
	{
		population.printStats();
		population[0]->neuralNet.printStats();
		population[0]->genome.printStats();
	}

	virtual void initPhenotypes() = 0;
	virtual void buildEnvironment() = 0;
	virtual bool isFinished() = 0;



	int simFrame;
	b2Vec2 renderCenter;
	//sWorld world;
	vector<sWorld> worlds;

};