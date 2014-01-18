// BoxSFML.cpp : Defines the entry point for the console application.
#include <sfml.h>
#include <Box2D\Box2D.h>
#include <vector>
#include <algorithm>
#include "State.h"
#include "sPhysics\sPhysics.h"
#include "sGraphics\sDebugDraw.h"
#include "sGraphics\sDisplayContainer.h"
#include "sEvolution\sPopulation.h"
#include "sGraphics\sGeneGraph.h"
#include "sGraphics\sFitnessGraph.h"
#include "Car.h"

#include <random>

using namespace std;


int n_cars = 50;
vector<Car*> cars;
sWorld world;
sConcavePolygon ground;
sPopulation population;

float32 worldWidth = 400;
float32 worldOffset = -6;
int width = 800, height = 600;
const float renderScale = 10.f;
float scale = 0.2f;
Car *leader;


float randRange(float32 s, float32 e)
{
	return s + (e - s) * float(rand())/RAND_MAX;
}



bool testForEnd()
{

	float bestFitness = -100;
	for(int i=0;i <cars.size(); i++){
		//if(cars[i]->alive){
			float fitness = cars[i]->getFitness();
			if(fitness > bestFitness){
				bestFitness = fitness;
				leader = cars[i];
			}
		//}
	}
	
	for(int i = 0; i < n_cars; i++){
		if(cars[i]->chassis.getPosition().x > worldWidth + worldOffset){
			cars[i]->die();
		}
	}

	Car *elite = leader;
	if(population.getGenerationCount() && population.getElites()){
		elite = (Car*)population[0];
	}

	//if(!leader->alive && !elite->alive) return true;

	for(int i = 0; i < n_cars; i++){
		if(cars[i]->alive) return false;
	}
	return true;
}


void createground()
{
	if(ground.isInWorld()){
		//return;
		world.remove(&ground);
		ground.resetShape();
	}
	const int avelen = 40;

	float start_h = 0.4f;

	float32 rollingAve[avelen] = { 0 };
	for(int i=0;i <avelen; i++){
		rollingAve[i] = start_h;
	}
	float32 total = float(avelen) * start_h;
	//srand(666);
	float32 len = worldWidth;
	float32 h = 30.f + 30.f * float(rand())/RAND_MAX;
	ground.add(len,h + 10.f);
	ground.add(0.f,h + 10.f);
	ground.setPosition(worldOffset, 5 - h * start_h);
	int index = 0;
	for(float32 x = 0; x <= len; x += 0.5f){
		float v = 1.0 * float(rand())/RAND_MAX;
		total -= rollingAve[index];
		total += v;
		rollingAve[index] = v;
		ground.add(x, (total / float(avelen)) * h);
		index = ++index % avelen;
	}
	ground.setType(STATIC_BODY);
	ground.finalizeShape();
	ground.setFriction(1);
	world.add(&ground);

	sf::Clock clck;
	srand(clck.getElapsedTime().asMicroseconds());
}



void plotGeneGraphs(vector<sGeneGraph> &geneGraphs)
{
	int n = 0;
	geneGraphs[n++].plotGene(population, "scale");
	geneGraphs[n++].plotGene(population, "chassisDensity");
	geneGraphs[n++].plotGene2D(population, "chassisScale_x", "chassisScale_y");

	geneGraphs[n++].plotGene(population, "frontMotorTorque");
	geneGraphs[n++].plotGene(population, "frontMotorSpeed");
	geneGraphs[n++].plotGene(population, "frontWheelRadius");
	geneGraphs[n++].plotGene(population, "frontWheelDensity");
	geneGraphs[n++].plotGene2D(population, "frontWheelPosition_x", "frontWheelPosition_y");
	geneGraphs[n++].plotGene2D(population, "frontWheelAnchor_x", "frontWheelAnchor_y");
	geneGraphs[n++].plotGene(population, "frontFrequencyHz");
	geneGraphs[n++].plotGene(population, "frontDampingRatio");

	geneGraphs[n++].plotGene(population, "rearMotorTorque");
	geneGraphs[n++].plotGene(population, "rearMotorSpeed");
	geneGraphs[n++].plotGene(population, "rearWheelRadius");
	geneGraphs[n++].plotGene(population, "rearWheelDensity");
	geneGraphs[n++].plotGene2D(population, "rearWheelPosition_x", "rearWheelPosition_y");
	geneGraphs[n++].plotGene2D(population, "rearWheelAnchor_x", "rearWheelAnchor_y");
	geneGraphs[n++].plotGene(population, "rearFrequencyHz");
	geneGraphs[n++].plotGene(population, "rearDampingRatio");


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


int main()
{
	sf::Clock clck;

	srand(clck.getElapsedTime().asMicroseconds());

	State state(world.b2world);

	population.setElites(1);

	cars.resize(n_cars);
	for(int i=0; i<n_cars; i++){
		cars[i] = new Car;
		population.addPhenotype(cars[i]);
		world.add(cars[i]);
	}

	createground();
	//world.add(&chain);

	sMouseJoint mouseJoint;
	mouseJoint.setBodyA(&ground);
	world.add(&mouseJoint);
	world.setGravity(b2Vec2(0,10));

	sf::Event e;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(width, height), "SFML works!", sf::Style::Default, settings);
	window.setPosition(sf::Vector2i(0,300));
	sf::View view;
	view.setSize(width, height);
	view.setCenter(0.5f, -0.5f);
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	sf::RenderStates renderState;
	
	sDebugDraw debugDraw(window);

	sf::Transform trans;
	//trans.translate(0.5f, -150.f);
	trans.scale(renderScale/scale, renderScale/scale);

	debugDraw.setTransform(trans);
	debugDraw.SetFlags(sDebugDraw::e_shapeBit | sDebugDraw::e_jointBit);
	
	//world.b2world.SetDebugDraw(&debugDraw);


	bool frameLimiter = true;
	double dt = 1000.f/60.f;
	double accumulator = 0;
	int frameCounter = 0;
	int lastTime =  clck.getElapsedTime().asMicroseconds();
	int simFrame = 0;;
	bool mouse_mode = true;
	bool mouseDown = false;;
	int renderTime = 0;
	int physicsTime = 0;
	int oldt2 = lastTime;
	int render_t = lastTime;
	bool render_flag = true;

	sFitnessGraph fitnessGraph;
	fitnessGraph.setPosition(2,44);
	vector<sGeneGraph> geneGraphs(19);
	plotGeneGraphs(geneGraphs);
	
	while(window.isOpen()){
		simFrame++;
		while(window.pollEvent(e)){
			if(e.type == sf::Event::Closed){
				window.close();
			} else if(e.type == sf::Event::Resized){
				width = window.getSize().x;
				height = window.getSize().y;
				view.setSize(width, height);
			} else if(e.type == sf::Event::KeyPressed){
				if(e.key.code == sf::Keyboard::F){
					frameLimiter = !frameLimiter;
					window.setFramerateLimit(frameLimiter ? 60 : 2000);
					window.setVerticalSyncEnabled(frameLimiter);
				} else if(e.key.code == sf::Keyboard::R){
					render_flag = !render_flag;
				}
			}
		}
		

		if(testForEnd()){
			simFrame = 0;

			population.newGeneration();
			for(int i = 0; i < population.size(); i++){
				world.remove(cars[i]);
				world.add(cars[i]);
			}
			createground();
			plotGeneGraphs(geneGraphs);
			fitnessGraph.renderGraph(population);
		}
		if(simFrame == 120){
			for(int i = 0; i < n_cars; i++){
				cars[i]->setAccelerator(1);
			}
		}
		int t2 = clck.getElapsedTime().asMicroseconds();

		world.step();

		physicsTime += clck.getElapsedTime().asMicroseconds() - t2;
		

		

		t2 = clck.getElapsedTime().asMicroseconds();
		if(render_flag){


			
			if(t2 - render_t > 1000000.f / 60.f || frameLimiter){
				b2Vec2 wp = b2Vec2(float(view.getCenter().x) / 50.f, float(view.getCenter().y) / 50.f);
				b2Vec2 p = leader->chassis.getPosition();
				p.y = wp.y + 0.1f * (p.y - wp.y);
				view.setCenter(p.x * 50, p.y * 50);				
				window.setView(view);
				debugDraw.setView(view);

				window.clear();
				debugDraw.prepare();
				debugDraw.DrawDebugData(world);

				debugDraw.finalize();

				window.setView(window.getDefaultView());

				drawGeneGraphs(window, geneGraphs);
				window.draw(fitnessGraph);

				window.display();

				render_t = t2;
			} else {
				//printf("skip render");
			}
		}
		renderTime += clck.getElapsedTime().asMicroseconds() - t2;
		
		frameCounter++;
		t2 = clck.getElapsedTime().asMicroseconds();
		if(t2 - lastTime > 999999){
			window.setTitle(to_string(frameCounter) + "fps " + to_string(renderTime/1000) + ":" + to_string(physicsTime/1000));
			frameCounter = 0;
			lastTime = t2;
			physicsTime = renderTime = 0;
		}
		

	}



	return 0;
}

