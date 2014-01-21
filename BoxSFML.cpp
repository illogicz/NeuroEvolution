// BoxSFML.cpp : Defines the entry point for the console application.
#include "Sims\RaceCar\RaceSim.h"
#include "sGraphics\sSimulationWindow.h"
#include "sNeuralNet\sNeuralNet.h"


int main()
{
	
	/*
	int runs = 20;
	while(runs--){
		sGenome genome;
		sNeuralNet neuralNet;
		neuralNet.create(genome);

		printf("=================================================\n\n");

		for(float i = -3.f; i < 3.f; i += 0.1f){

			neuralNet.inputs[0].value = i;
			neuralNet.run();
			printf("%f -> %f \n", i, neuralNet.outputs[0].activation());

		}
	}*/

	

	srand(536855);

	RaceSim simulation;
	sSimulationWindow window;
	

	simulation.populationSize = 130;
	simulation.elites = 4;
	simulation.mutationRate = 0.005f;
	simulation.selectionBias = 1.7f;
	simulation.breadingPoolFraction = 0.9f;

	simulation.maxRoughness = 20;
	simulation.minRoughness = 20;
	//simulation
	simulation.randomizeEnvironment = true;
	
	window.setSimulation(&simulation);

	window.start();
	
	return 0;

}

