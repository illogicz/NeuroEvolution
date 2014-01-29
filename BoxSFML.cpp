
#include "Sims\RaceCar\Car.h"
#include "Sims\RaceCar\Worm.h"
#include "Sims\RaceCar\Biped.h"
#include "Sims\RaceCar\TetraPod.h"
#include "Sims\RaceCar\RaceSimulation.h"
#include "Sims\RaceCar\SwarmSimulation.h"
#include "sGraphics\sSimulationWindow.h"


void runBipedSimulation()
{

	sRandom::seed(124555);


	RaceSimulation<TetraPod> simulation;
	sSimulationWindow window;

	simulation.population.setWinnersPerPrelim(5);
	simulation.populationSize = 50;
	simulation.elites = 0;
	simulation.mutationRate = 0.007f;
	simulation.selectionBias = 1.8f;
	simulation.breadingPoolFraction = 0.9f;

	//simulation.groundSegmentSize = 0.1f;
	simulation.groundFrequency = 20.f;
	simulation.maxRoughness = 0;
	simulation.minRoughness = 0;
	simulation.randomizeEnvironment = false;
	simulation.renderScale = 50;
	simulation.worldWidth = 800;
	simulation.worldOffset_y = 0.f;
	window.setSimulation(&simulation);

	window.start();

}

void runSwarmSimulation()
{
	sRandom::seed(1000002);


	SwarmSimulation simulation;
	sSimulationWindow window;


	// Population and selection settings
	simulation.populationSize = 50;
	simulation.elites = 0;
	simulation.mutationRate = 0.007f;
	simulation.selectionBias = 2.f;
	simulation.breadingPoolFraction = 0.5f;
	

	//simulation.groundType = RaceSimulation<Worm>::GroundType::SinWaves;

	// Display
	simulation.renderScale = 13;
	window.setSimulation(&simulation);
	window.start();

}

void runWormSimulation()
{
	sRandom::seed(1339);


	RaceSimulation<Worm> simulation;
	sSimulationWindow window;

	// Worm setup
	Worm::touchSense = true;
	Worm::bodySense = true;
	Worm::maxNeuronBias = 1;
	Worm::maxSynapseWeight = 3;//.f / tanh(1);
	Worm::muscleTorque = 25; // 20
	
	Worm::progressTimeout = 6000;
	Worm::progressAmount = 0.1;
	
	Worm::killOnLackOffProgress = true;
	Worm::killOnStarvation = true;
	Worm::startEnery = 10000;

	// Population and selection settings
	simulation.populationSize = 50;
	simulation.elites = 0;
	simulation.mutationRate = 0.007f;
	simulation.selectionBias = 1.9f;
	simulation.breadingPoolFraction = 0.9f;
	simulation.population.setWinnersPerPrelim(10);
	simulation.fitnessFunction.useSpeed = true;
	simulation.fitnessFunction.absolueDistance = false;
	//simulation.setGravity(0,0);
	//simulation.fit

	// Ground
	simulation.groundType = RaceSimulation<Worm>::Steps;
	simulation.worldOffset_y = 1.f;
	simulation.worldOffset = 10;

	simulation.maxRoughness = 10.f;
	simulation.minRoughness = 10.f;

	simulation.firstStepHeight = 0.0f;
	simulation.stepIncrease = 0.0f;

	simulation.groundSegmentSize = 4.f;
	simulation.randomizeEnvironment = false;
	simulation.worldWidth = 1000;
	simulation.groundFrequency = 2.1f;


	//simulation.groundType = RaceSimulation<Worm>::GroundType::SinWaves;

	// Display
	simulation.renderScale = 60;
	window.setSimulation(&simulation);

		//simulation.population[0]->neuralNet.analyze();


	window.start();
	

}


void runCarSimulation()
{
	sRandom::seed(12455);


	RaceSimulation<Car> simulation;
	sSimulationWindow window;


	Car::bodyType = Car::BodyType::Fixed;


	simulation.populationSize = 90;
	simulation.elites = 1;
	simulation.mutationRate = 0.005f;
	simulation.selectionBias = 2.f;
	simulation.breadingPoolFraction = 0.8f;

	simulation.maxRoughness = 20;
	simulation.minRoughness = 20;
	simulation.randomizeEnvironment = true;
	

	window.setSimulation(&simulation);

	window.start();

}

int main()
{
	//runBipedSimulation();
	//runSwarmSimulation();
	runWormSimulation();
	runCarSimulation();
	
	return 0;

}