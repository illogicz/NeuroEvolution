
#include "Sims\RaceCar\Car.h"
#include "Sims\RaceCar\Worm.h"
#include "Sims\RaceCar\Biped.h"
#include "Sims\RaceCar\TetraPod.h"
#include "Sims\RaceCar\RaceSimulation.h"
#include "Sims\RaceCar\SwarmSimulation.h"
#include "Sims\RaceCar\TopDownRaceSimulation.h"
#include "Sims\RaceCar\PoleBalanceSimulation.h"
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
	simulation.maxRoughness = 1;
	simulation.minRoughness = 1;
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
	sRandom::seed(13523345);


	RaceSimulation<Worm> simulation;
	sSimulationWindow window;

	// Worm setup
	Worm::touchSense = false;
	Worm::bodySense = true;
	Worm::muscleModel = true;
	Worm::muscleTorque = 30; // 20
	
	Worm::progressTimeout = 3600;
	Worm::progressAmount = 2;
	
	Worm::killOnLackOffProgress = false;
	Worm::killOnStarvation = false;
	Worm::maxLifeTime = 600;
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
	simulation.setGravity(0,7.f);
	//simulation.fit

	// Ground
	simulation.groundType = RaceSimulation<Worm>::GroundType::PerlinNoise;
	simulation.worldOffset_y = 6.f;
	simulation.worldOffset = 10;

	simulation.maxRoughness = 50.f;
	simulation.minRoughness = 50.f;

	simulation.firstStepHeight = 0.0f;
	simulation.stepIncrease = 0.0f;

	simulation.groundSegmentSize = 0.3f;
	simulation.randomizeEnvironment = true;
	simulation.groundRampup = 400;
	simulation.worldWidth = 400;
	simulation.groundFrequency = 3.1f;
	simulation.perlinFrequency = 8;
	simulation.perlinOctaves = 8;
	//simulation.groundType = RaceSimulation<Worm>::GroundType::SinWaves;

	// Display
	simulation.renderScale = 60;
	window.setSimulation(&simulation);

		//simulation.population[0]->neuralNet.analyze();


	window.start();
	

}


void runCarSimulation()
{
	sRandom::seed(128955);


	RaceSimulation<Car> simulation;
	sSimulationWindow window;


	Car::bodyType = Car::BodyType::Fixed;


	simulation.populationSize = 50;
	simulation.elites = 0;
	simulation.mutationRate = 0.007f;
	simulation.selectionBias = 2.f;
	simulation.breadingPoolFraction = 1.f;
	simulation.population.setWinnersPerPrelim(5);

	simulation.maxRoughness = 5;
	simulation.minRoughness = 5;
	simulation.randomizeEnvironment = true;
	
	simulation.perlinFrequency = 4;
	simulation.worldOffset_y = 3;
	simulation.groundSegmentSize = 1;
	simulation.groundRampup = 2000;
	simulation.worldWidth = 2000;
	simulation.perlinOctaves = 8;
	simulation.worldOffset = 7;
	window.setSimulation(&simulation);

	window.start();

}

void runTopDownRace()
{
	sRandom::seed(128955);

	TopDownRaceSimulation simulation;
	sSimulationWindow window;
	simulation.mutationRate = 0.005;
	simulation.elites = 0;

	window.setSimulation(&simulation);
	window.start();

}

void runPoleBalancing()
{
	sRandom::seed(128955);

	PoleBalanceSimulation simulation;
	sSimulationWindow window;
	simulation.mutationRate = 0.005;
	simulation.elites = 0;

	window.setSimulation(&simulation);
	window.start();

}

int main()
{
	runPoleBalancing();
	//runTopDownRace();
	//runBipedSimulation();
	//runSwarmSimulation();
	runWormSimulation();
	runCarSimulation();
	
	return 0;

}