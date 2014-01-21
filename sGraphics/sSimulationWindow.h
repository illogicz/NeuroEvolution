#include "..\Sims\RaceCar\RaceSim.h"
#include "sSimulationDisplay.h"
#include "sGeneGraph.h"
#include "sFitnessGraph.h"


class sSimulationWindow
{

public:

	sSimulationWindow()
	{

		width = 800;
		height = 600;
		frameRate = 60;
		frameLimiter = true;
		render_flag = true;


		settings.antialiasingLevel = 8;
		window.create(sf::VideoMode(width, height), "", sf::Style::Default, settings);
		window.setPosition(sf::Vector2i(0,300));
		window.setFramerateLimit(frameRate);
		window.setVerticalSyncEnabled(true);


		simulationDisplay.setSize(width, height);

		view.setSize(width, height);
		view.setCenter(width/2, height/2);


		fitnessGraph.setPosition(2,44);
	}

	void setSize(int width, int height)
	{
		window.setSize(sf::Vector2u(width, height));
		simulationDisplay.setSize(width, height);
		view.setSize(width, height);
		view.setCenter(width/2, height/2);
	}


	void setSimulation(sSimulation *simulation)
	{
		m_simulation = simulation;
		m_simulation->init();

		geneGraphs.resize(simulation->population[0]->genome.size());
		plotGeneGraphs(geneGraphs);

	}

	void start()
	{
		frameCounter = 0;
		physicsCounter = 0;
		lastSecondTime =  clck.getElapsedTime().asMicroseconds();
		while(true){
			run();
		}
	}

private:

	// Settings
	sSimulation *m_simulation;
	float width;
	float height;
	bool frameLimiter;
	bool render_flag;
	int frameRate;

	// Display
	sSimulationDisplay simulationDisplay;
	sf::Event e;
	sf::ContextSettings settings;
	sf::RenderWindow window;
	sf::View view;
	sf::RenderStates renderState;
	
	// Graphs
	sFitnessGraph fitnessGraph;
	vector<sGeneGraph> geneGraphs;	






	void plotGeneGraphs(vector<sGeneGraph> &geneGraphs)
	{
		int n = 0;
		geneGraphs[n++].plotGene(m_simulation->population, "scale");
		geneGraphs[n++].plotGene(m_simulation->population, "chassisDensity");
		geneGraphs[n++].plotGene2D(m_simulation->population, "chassisScale_x", "chassisScale_y");

		geneGraphs[n++].plotGene(m_simulation->population, "frontMotorTorque");
		geneGraphs[n++].plotGene(m_simulation->population, "frontMotorSpeed");
		geneGraphs[n++].plotGene(m_simulation->population, "frontWheelRadius");
		geneGraphs[n++].plotGene(m_simulation->population, "frontWheelDensity");
		geneGraphs[n++].plotGene2D(m_simulation->population, "frontWheelPosition_x", "frontWheelPosition_y");
		geneGraphs[n++].plotGene2D(m_simulation->population, "frontWheelAnchor_x", "frontWheelAnchor_y");
		geneGraphs[n++].plotGene(m_simulation->population, "frontFrequencyHz");
		geneGraphs[n++].plotGene(m_simulation->population, "frontDampingRatio");

		geneGraphs[n++].plotGene(m_simulation->population, "rearMotorTorque");
		geneGraphs[n++].plotGene(m_simulation->population, "rearMotorSpeed");
		geneGraphs[n++].plotGene(m_simulation->population, "rearWheelRadius");
		geneGraphs[n++].plotGene(m_simulation->population, "rearWheelDensity");
		geneGraphs[n++].plotGene2D(m_simulation->population, "rearWheelPosition_x", "rearWheelPosition_y");
		geneGraphs[n++].plotGene2D(m_simulation->population, "rearWheelAnchor_x", "rearWheelAnchor_y");
		geneGraphs[n++].plotGene(m_simulation->population, "rearFrequencyHz");
		geneGraphs[n++].plotGene(m_simulation->population, "rearDampingRatio");


		for(int i = 0; i < n; i++){
			geneGraphs[i].setPosition(i * 42 + 2, 2);
		}
	}

	void drawGeneGraphs(sf::RenderWindow &window, vector<sGeneGraph> &geneGraphs)
	{
		for(int i = 0; i < geneGraphs.size(); i++){
			window.draw(geneGraphs[i]);
		}
	}


	// Frame Timings
	sf::Clock clck;
	int frameCounter;
	int physicsCounter;
	int lastSecondTime;
	int lastPhysicsTime;



	int now()
	{
		return clck.getElapsedTime().asMicroseconds();
	}

	bool stepSimulation()
	{
		bool newGen = m_simulation->step();
		if(newGen){
			simulationDisplay.setCenter(0,0);
			fitnessGraph.renderGraph(m_simulation->population);
			plotGeneGraphs(geneGraphs);
		}
		physicsCounter++;
		return newGen;
	}

	void run()
	{

		//-----------------------------------------------------------------------------------
		// Handle user inputs
		//-----------------------------------------------------------------------------------

		// Return when window closes
		if(!window.isOpen()) return;

		while(window.pollEvent(e)){

			// Window closing
			if(e.type == sf::Event::Closed){
				window.close();

			// Window resized
			} else if(e.type == sf::Event::Resized){

				setSize(window.getSize().x, window.getSize().y);

			// Keyboard input
			} else if(e.type == sf::Event::KeyPressed){

				// 
				if(e.key.code == sf::Keyboard::F){

					frameLimiter = !frameLimiter;
					window.setFramerateLimit(frameLimiter ? 60 : 3000);
					window.setVerticalSyncEnabled(frameLimiter);


				} else if(e.key.code == sf::Keyboard::R){
					render_flag = !render_flag;

				} else if(e.key.code == sf::Keyboard::T){
					simulationDisplay.toggleFocus();
				}
			} else if(e.type == sf::Event::MouseButtonPressed || 
				      e.type == sf::Event::MouseButtonReleased){
				
			}
		}


		//-----------------------------------------------------------------------------------
		// Run Simulation
		//-----------------------------------------------------------------------------------

		bool newGen = stepSimulation();


		
		// If framelimiter is off loop until time runs out or maxsteps has been reached
		int maxSteps = 500;
		while((now() < lastPhysicsTime + 1000000.f / frameRate)
			    && (m_simulation->speedUp || !frameLimiter)){

			newGen |= stepSimulation();
			if(!--maxSteps)break;
		}
		lastPhysicsTime = now();
		

		//-----------------------------------------------------------------------------------
		// Render Simulation
		//-----------------------------------------------------------------------------------

		// Draw simultion
		if(render_flag || newGen){

			// Clear window
			window.clear();

			// Draw simulation
			simulationDisplay.draw(*m_simulation, &window, sf::RenderStates::Default);

			// Reset view
			window.setView(view);

			// Draw Gene graphs
			drawGeneGraphs(window, geneGraphs);
				
			// Draw Fitness Graph
			window.draw(fitnessGraph);

			// Display Contents
			window.display();
		}


		//-----------------------------------------------------------------------------------
		// Framerate and Timing
		//-----------------------------------------------------------------------------------

		frameCounter++;
		if(now() - lastSecondTime > 999999){
			window.setTitle(to_string(frameCounter) + " : " + to_string(physicsCounter) + " fps");
			frameCounter = physicsCounter = 0;
			lastSecondTime = now();
		}
		

	}

};