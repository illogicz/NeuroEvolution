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

	

	srand(3453455);

	RaceSim simulation;
	sSimulationWindow window;
	

	simulation.elites = 2;
	simulation.mutationRate = 0.000f;
	simulation.selectionBias = 2.f;
	simulation.breadingPoolFraction = 1.f;

	simulation.maxRoughness = 20;
	simulation.minRoughness = 20;
	//simulation
	simulation.randomizeEnvironment = false;
	
	window.setSimulation(&simulation);

	window.start();
	
	return 0;

}

