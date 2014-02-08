#include "sSimulationDisplay.h"
#include "sGeneGraph.h"
#include "sFitnessGraph.h"
#include "sPopulationGeneticsDisplay.h"
#include "sNeuralNetDisplay.h"
#include "sText.h"
#include <Windows.h>
#include "sNeuralNet\sNeuralNetAnalyser.h"

class sSimulationWindow
{

public:

	sSimulationWindow()
	{

		width = 1280;
		height = 720;
		frameRate = 60;
		frameLimiter = true;
		render_flag = true;
		render_ui = true;
		render_graphs = true;
		m_mouseDown = false;
		m_message = "";
		messageTimeout = 0;


		settings.antialiasingLevel = 4;
		window.create(sf::VideoMode(unsigned int(width), unsigned int(height)), "", sf::Style::Default, settings);
		window.setPosition(sf::Vector2i(0,150));
		window.setFramerateLimit(frameRate);
		//window.setVerticalSyncEnabled(true);

		//generationText.
		//neuralNetDisplay.setSize(300,300);

		sText::loadFont("consola.ttf");


		
		
	}

	void setSize(int w, int h)
	{
		width = w;
		height = h;
		window.setSize(sf::Vector2u(width, height));
		layoutUI();
	}


	void setSimulation(sSimulation *simulation)
	{
		m_simulation = simulation;
		m_simulation->init();
		simulationDisplay.setSimulation(simulation);

		geneDisplay.init(simulation->population);
		geneDisplay.plot();

		layoutUI();



		sNeuralNet &ann = simulation->population[0]->neuralNet;
		nnAnalyser1.graphWidth = 210;
		nnAnalyser1.height = 100;
		nnAnalyser1.analyzeNetwork(ann);

		nnAnalyser2.graphWidth = nnAnalyser1.graphWidth / (ann.getLayerCount() - 1);
		nnAnalyser2.height = 50;
		nnAnalyser2.analyseLayers(ann);

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
	bool render_ui;
	bool render_graphs;
	int frameRate;

	// Display
	sSimulationDisplay simulationDisplay;
	sf::Event e;
	sf::ContextSettings settings;
	sf::RenderWindow window;
	sf::View view;
	sf::RenderStates renderState;
	sf::Font font;
	b2Vec2 mousePosition;

	// Graphs
	sFitnessGraph fitnessGraph;
	vector<sGeneGraph> geneGraphs;	
	sPopulationGeneticsDisplay geneDisplay;
	sNeuralNetDisplay neuralNetDisplay;
	sNeuralNetworkAnalyser nnAnalyser1;
	sNeuralNetworkAnalyser nnAnalyser2;

	void drawUI()
	{

		drawMessage();

		if(!render_ui)return;

		// Draw Graphs

		if(render_graphs){
			window.draw(fitnessGraph);

			geneDisplay.mousePosition = sf::Vector2f(mousePosition.x, mousePosition.y);
			window.draw(geneDisplay);

			sNeuralNet &neuralNet = m_simulation->population[simulationDisplay.getFocusRank()]->neuralNet;
			neuralNetDisplay.renderNeuralNet(window, neuralNet);

			nnAnalyser2.setPosition(neuralNetDisplay.getPosition().x - nnAnalyser2.width, height - nnAnalyser2.height - nnAnalyser1.height);
			nnAnalyser2.draw(window);
			nnAnalyser1.setPosition(neuralNetDisplay.getPosition().x - nnAnalyser1.width, height - nnAnalyser1.height);
			nnAnalyser1.draw(window);
		}

		int popSize = m_simulation->population.size();
		if(m_simulation->population.prelimsComplete()){
			drawText("Generation: " + to_string(m_simulation->population.getGenerationCount() + 1), 10,44);
			drawText("Population: " + to_string(popSize), 10,62);
		} else {
			drawText("Generation: Getting initial seed genes", 10,44);
			int prelimSize = m_simulation->population.getPrelimWinnerCount();
			drawText("Population: " + to_string(prelimSize) + " / " + to_string(popSize), 10,62);
		}
		

		if((m_simulation->speedUp || !frameLimiter)){
			sf::CircleShape triangle(17, 3);
			triangle.setFillColor(sf::Color(200,200,200));
			triangle.setRotation(90);
			triangle.setPosition(width / 2 - 13, 50);
			window.draw(triangle);			
			triangle.setPosition(width / 2 + 13, 50);
			window.draw(triangle);
		}
	}

	void layoutUI()
	{
		simulationDisplay.setSize(width, height);

		view.setSize(width, height);
		view.setCenter(width/2, height/2);

		fitnessGraph.setPosition(2,2);
		fitnessGraph.setSize(width - 4, 40);


		neuralNetDisplay.setPosition(width - neuralNetDisplay.width, height - neuralNetDisplay.height);

		geneDisplay.setPosition(0, height - geneDisplay.height * 2);
		geneDisplay.setScale(2,2);

	}

	bool hasFocus()
	{

		HWND handle = window.getSystemHandle();
		bool one = handle == GetFocus();
		bool two = handle == GetForegroundWindow();

		if(one != two) 
		{
			SetFocus(handle);
			SetForegroundWindow(handle);
		}

		return one && two;
	}

	void setFocus()
	{
		HWND handle = window.getSystemHandle();
		SetFocus(handle);
	}

	void drawText(string text, float x, float y, float a = 255, bool center = false)
	{
		sText::drawText(window, text, x, y, 18, sf::Color(255,255,255,a), center);
	}

	void showMessage(string message)
	{
		m_message = message;
		messageTimeout = 150;
	}
	void drawMessage()
	{
		if(messageTimeout){
			messageTimeout--;
			int a = 255;
			if(messageTimeout < 30){
				a *= messageTimeout / 30.f;
			}
			drawText(m_message, width / 2, 120, a, true);
		}
		
	}
	string m_message;
	int messageTimeout;

	

	// Frame Timings
	sf::Clock clck;
	int frameCounter;
	int physicsCounter;
	unsigned long long lastSecondTime;
	unsigned long long lastPhysicsTime;
	bool m_mouseDown;
	//bool m_staticView;

	unsigned long long now()
	{
		return clck.getElapsedTime().asMicroseconds();
	}

	bool stepSimulation()
	{
		simulationDisplay.clearDebugDraw();
		bool newGen = m_simulation->step();
		if(newGen){
			simulationDisplay.setCenter(0,0);
			fitnessGraph.renderGraph(m_simulation->population);
			geneDisplay.plot();
			//plotGeneGraphs(geneGraphs);
		}
		physicsCounter++;
		return newGen;
	}

	//=======================================================================================
	// Main loop
	//=======================================================================================


	void run()
	{

		//-----------------------------------------------------------------------------------
		// Handle user inputs
		//-----------------------------------------------------------------------------------

		// Return when window closes
		if(!window.isOpen()) return;

		
		mousePosition.x = sf::Mouse::getPosition(window).x;
		mousePosition.y = sf::Mouse::getPosition(window).y;


		while(window.pollEvent(e)){

			// Window closing
			if(e.type == sf::Event::Closed){
				window.close();

			// Window resized
			} else if(e.type == sf::Event::Resized){

				setSize(window.getSize().x, window.getSize().y);

			// Keyboard input
			} else if(e.type == sf::Event::KeyPressed){

				// Framerate control
				if(e.key.code == sf::Keyboard::Space){

					frameLimiter = !frameLimiter;
					window.setFramerateLimit(frameLimiter ? frameRate : 3000);

				} else if(e.key.code == sf::Keyboard::R){
					render_flag = !render_flag;

				} else if(e.key.code == sf::Keyboard::U){
					render_ui = !render_ui;

				} else if(e.key.code == sf::Keyboard::G){
					render_graphs = !render_graphs;

				} else if(e.key.code == sf::Keyboard::V){
					simulationDisplay.toggleStaticView();

				} else if(e.key.code == sf::Keyboard::F){
					frameRate = 90 - frameRate;
					window.setFramerateLimit(frameLimiter ? frameRate : 3000);
					showMessage("Framerate: " + to_string(frameRate));


				// View targeting
				} else if(e.key.code == sf::Keyboard::Home){
					simulationDisplay.focusFirst(); 

				} else if(e.key.code == sf::Keyboard::End){
					simulationDisplay.focusLast();

				} else if(e.key.code == sf::Keyboard::PageUp){
					simulationDisplay.focusNext();

				} else if(e.key.code == sf::Keyboard::PageDown){
					simulationDisplay.focusPrev();

				} else if(e.key.code == sf::Keyboard::Delete){
					simulationDisplay.getFocusTarget()->die();

				} else if(e.key.code == sf::Keyboard::Insert){
					simulationDisplay.toggleLockFocus();
					if(simulationDisplay.getLockFocus()){
						showMessage("View: Individual");
					} else {
						showMessage("View: Rank");
					}

				// Elites
				} else if(e.key.code == sf::Keyboard::Up){
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
						int e = m_simulation->population.getElites() + 1;
						m_simulation->population.setElites(e);
						showMessage("Elites:"+to_string(m_simulation->population.getElites()));

					} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::M)){
						m_simulation->population.increaseMutationRate();
						showMessage("Mutation Rate:"+to_string(m_simulation->population.getMutationRate()));
					}

				}  else if(e.key.code == sf::Keyboard::Down){
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
						int e = m_simulation->population.getElites() - 1;
						m_simulation->population.setElites(e);
						showMessage("Elites:"+to_string(m_simulation->population.getElites()));

					} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::M)){
						m_simulation->population.decreaseMutationRate();
						showMessage("Mutation Rate:"+to_string(m_simulation->population.getMutationRate()));
					}
				}

			} else if(e.type == sf::Event::MouseButtonPressed){ 

				setFocus();
				if(e.mouseButton.button == sf::Mouse::Button::Left && !m_mouseDown){
					simulationDisplay.mousePressed(mousePosition);
					m_mouseDown = true;
				}

			} else if(e.type == sf::Event::MouseButtonReleased){

				if(e.mouseButton.button == sf::Mouse::Button::Left && m_mouseDown){			
					simulationDisplay.mouseReleased(mousePosition);
					m_mouseDown = false;
				}

			} else if(e.type == sf::Event::MouseMoved){

				simulationDisplay.mouseMoved(mousePosition);

			}
		}


		// Release outside window
		if(m_mouseDown && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
			simulationDisplay.mouseReleased(mousePosition);
			m_mouseDown = false;
		}

		//-----------------------------------------------------------------------------------
		// Run Simulation
		//-----------------------------------------------------------------------------------

		bool newGen = stepSimulation();
		if(frameRate == 30)newGen = stepSimulation();

		
		// If framelimiter is off loop until time runs out or maxsteps has been reached
		int maxSteps = 500;
		while(   (now() < lastPhysicsTime + 1000000.f / frameRate) // step as long as there is time left
			  && (m_simulation->speedUp || !frameLimiter)          // only step if frame limiting is off
			  && !newGen                                           // stop if there is a new generation
			){

			newGen |= stepSimulation();
			if(!--maxSteps)break;
		}
		lastPhysicsTime = now();
		

		//-----------------------------------------------------------------------------------
		// Rendering
		//-----------------------------------------------------------------------------------

		// Draw simultion
		if(render_flag || newGen){

			// Clear window
			window.clear();

			if(!render_ui){
				simulationDisplay.clearDebugDraw();
			}

			// Draw simulation
			simulationDisplay.draw(&window, sf::RenderStates::Default);

			// Reset view
			window.setView(view);


			drawUI();

			// Display Contents
			window.display();

		}


		//-----------------------------------------------------------------------------------
		// Framerate Timing
		//-----------------------------------------------------------------------------------

		frameCounter++;
		if(now() - lastSecondTime > 999999){
			window.setTitle(to_string(frameCounter) + " : " + to_string(physicsCounter) + " fps");
			frameCounter = physicsCounter = 0;
			lastSecondTime = now();
		}
		

	}

};