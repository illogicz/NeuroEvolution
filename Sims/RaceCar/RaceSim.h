#pragma once
#include "../../sEvolution/sSimulation.h"
#include "Car.h"
#include "../../sUtils/perlin.h"

class RaceSim : public sSimulation
{

public:

	RaceSim()
	{
		worldWidth = 600;
		worldOffset = 6;
		populationSize = 50;
		randomizeEnvironment = true;
		minRoughness = 4;
		maxRoughness = 4;

		ground.vertexSplitBaseWeight = 1.f;
		ground.doubleSolveWeight = 2.f;
		ground.segmentSplitBaseWeight = 10.f;
		ground.lengthWeight = 0.5f;
		ground.dotWeight = 1.f;

	}

	~RaceSim()
	{
		for(int i = 0; i < populationSize; i++){
			delete population[i];
		}
	}


	void initPhenotypes()
	{
		for(int i = 0; i < populationSize; i++){
			Car *car = new Car;
			car->init(world);
			population.addPhenotype(car);
			//world.addContactListener(car, &car->chassis);
			world.add(car);
		}
	}


	// Return whether the simulation is finished
	// In this case it's when all phenotypes are no longer active/alive
	bool isFinished()
	{
		for(int i = 0; i < population.size(); i++){
			if(population[i]->getPosition().x > worldWidth - worldOffset){
				population[i]->die();
				return true;
			}
		}

		// Speed up simulation if leader is not alive
		if(leader != nullptr)speedUp = !leader->alive;

		for(int i = 0; i < population.size(); i++){
			if(population[i]->alive) return false;
		}
		return true;
	}

	// Build / Rebuild environment
	// Builds the ground based on roughness inputs
	void buildEnvironment()
	{
		if(ground.isInWorld()){
			// Don't regenrate if we don;t randomize
			if(!randomizeEnvironment)return;

			// Remove and reset
			world.remove(&ground);
			ground.resetShape();
		}

		Perlin perlin(5, 8, 0.5, rand());


		randOffset1 = rand() % int(worldWidth);
		randOffset2 = rand() % int(worldWidth);
		randOffset3 = rand() % int(worldWidth);



		float32 h = minRoughness + (maxRoughness - minRoughness) * float(rand())/RAND_MAX;
		float32 start_h = getHeightValue(worldOffset, perlin) * h;
		ground.add(worldWidth, h + 0.0001f);
		ground.add(-2.f, h + 0.0001f);
		ground.add(-2.f, start_h - 1);
		ground.add(-1.f, start_h);
		ground.add(worldOffset, start_h);
		ground.setPosition(3.f - worldOffset, 5 - start_h);
		int index = 0;
		float rampup = 50;
		for(float32 x = worldOffset; x <= worldWidth; x += 0.5f){

			float y = getHeightValue(x, perlin) * h;
			if(x < rampup){
				y = start_h + (y - start_h) * (x / rampup);
			}
			ground.add(x, y);
		}
		ground.finalizeShape();
		ground.setType(STATIC_BODY);
		ground.setFriction(1);
		world.add(&ground);

		speedUp = false;
	}





	// Simulation Settings
	float worldWidth;
	float worldOffset;
	int populationSize;
	bool randomizeEnvironment;
	float minRoughness;
	float maxRoughness;

	float groundFrequency;
	float groundOctaves;


private:

	// returns a value between 0 - 1;
	float getHeightValue(float x, Perlin &perlin)
	{
		x *= 2;
		float v;
		if(true){
			v = perlin.Get(x / worldWidth, 0);
		} else {
			float w1 = sin(randOffset1 + x / worldWidth * 59) * 0.25f;
			float w2 = sin(randOffset2 + x / worldWidth * 231) * 0.15f;
			float w3 = sin(randOffset3 + x / worldWidth * 851) * 0.05f;
			v = w1 + w2 + w3;
		}
		//if(v < 0) return 0.5 - v * v * 2;
		//return v * v * 2 + 0.5;
		return 0.5f + v;
		
	}
	float randOffset1, randOffset2, randOffset3;

	sConcavePolygon ground;
};