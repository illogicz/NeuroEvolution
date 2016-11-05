#pragma once
#include "../sEvolution/sSimulation.h"
#include "Phenotypes/PoleCart.h"
#include "Phenotypes/CircleBalance.h"
#include "../sPhysics/sEdgeRectangle.h"
#include "../sPhysics/sCircle.h"
#include "../sUtils/perlin.h"
/*

*/

class PoleBalanceSimulation : public sSimulation
{

public:

	PoleBalanceSimulation()
	{


		rows = 6;
	    cols = 10;


		width = 150;
		height = 12;

		built = false;
		staticView = true;
		renderScale = 10;

		populationSize = rows * cols;
	}

	~PoleBalanceSimulation()
	{
		for(int i = 0; i < populationSize; i++){
			delete population[i];
		}
	}
	
	int populationSize;
	//float simulationTime;
	float width;
	float height;

	int rows;
	int cols;

protected:

	void initPhenotypes()
	{

		int w = (int)ceil(sqrt(populationSize)) + 1;
		int h = (int)floor(float(populationSize) / w);

		for(int i = 0; i < rows; i++){
			for(int j = 0; j < cols; j++){
				CircleBalance *cart = new CircleBalance;
				cart->position.y = i * 12.f - 24.f;
				cart->position.x = float(j) / (cols - 1) * 100 - 50;
				//cart->groundBody = &groundBodies[i];
				addPhenotype(cart);
			}
		}
	}


	// Step fucntion
	// Return whether the simulation is finished
	// In this case it's when all phenotypes are no longer active/alive
	bool isFinished()
	{

		for(size_t i = 0; i < population.size(); i++){
			if(population[i]->alive) return false;
		}

		return true;
	}

	// Build / Rebuild environment
	// Builds the ground based on roughness inputs

	vector<sEdgeRectangle> groundBodies;
	bool built;
	void buildEnvironment()
	{
		if(built) return;

		groundBodies.resize(rows);
		for(int i = 0; i < rows; i++){
			groundBodies[i].setSize(width,height);
			groundBodies[i].setFriction(2);
			groundBodies[i].setPosition(0, i * 12.f - 30.f);
			groundBodies[i].setType(STATIC_BODY);
			//world.add(&groundBodies[i]);
		}


		//world.setGroundBody(&groundBodies[0]);
		//world.add(&groundBodies[0]);


	}
	sEdgeRectangle ground;


};