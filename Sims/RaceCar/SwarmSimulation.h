#pragma once
#include "../../sEvolution/sSimulation.h"
#include "Worm.h"
#include "../../sPhysics/sEdgeRectangle.h"
#include "../../sUtils/perlin.h"

class sSwarmFitness : public sFitnessFunction
{
public: 
	 b2Vec2 target;
	 float operator()(sPhenotype *phenotype)
	{
		//if(phenotype->isLeader()){
		//	printf("d = %f \n", (phenotype->getPosition() - target).Length());
		//}

		float distance_traveled = phenotype->getPosition().Length();
		float distance_target = (phenotype->getPosition() - target).Length() ;
		return 25 - distance_target + distance_traveled * 0.5;
	}
};

class SwarmSimulation : public sSimulation
{

public:

	SwarmSimulation()
	{

		ground.setSize(1000,1000);

		setGravity(0,0);
		world.setGroundBody(&ground);
		world.add(&ground);

		simulationTime = 100000;

		staticView = true;

		target.setIsSensor(true);
		target.setRadius(1);

		world.add(&target);
	}

	~SwarmSimulation()
	{
		for(int i = 0; i < populationSize; i++){
			delete population[i];
		}
	}

	int populationSize;
	float simulationTime;

	sSwarmFitness fitnessFunction;
	sCircle target;

protected:

	void initPhenotypes()
	{


		for(int i = 0; i < populationSize; i++){
			Worm *worm = new Worm;

			worm->bodySense = true;
			worm->directionSense = true;
			worm->fitnessFunction = &fitnessFunction;
			worm->progressTimeout = 3000;
			worm->maxNeuronBias = 0;
			worm->maxSynapseWeight = 3;
			worm->muscleSpeed = 0.1;
			worm->muscleTorque = 10;
			worm->lateralDamping = 0.9;


			//worm->pulseFeedback = true;
			worm->init(world);
			population.addPhenotype(worm);

			world.add(worm);
			if(!i){
				worm->neuralNet.printStats();
				worm->genome.printStats();
			}
		}
		population.printStats();

	}


	// Return whether the simulation is finished
	// In this case it's when all phenotypes are no longer active/alive
	bool isFinished()
	{

		if(world.getBodiesAt(target.getPosition()).size() > 1){
			return true;
		}

		for(int i = 0; i < population.size(); i++){
			if(population[i]->alive) return false;
		}
		return true;
	}

	// Build / Rebuild environment
	// Builds the ground based on roughness inputs
	void buildEnvironment()
	{
		float da = population.getGenerationCount() / 100.f;
		b2Vec2 p(25, 0);
		b2Rot a(sRandom::getFloat(-da, da));
		p = b2Mul(a, p);
		target.setPosition(p);

		fitnessFunction.target = p;

		for(int i = 0; i < populationSize; i++){
			((Worm*)population[i])->directionTarget = p;
		}

	}


private:

	sEdgeRectangle ground;
};