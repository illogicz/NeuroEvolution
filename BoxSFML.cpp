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
#include "Car.h"

#include <random>

using namespace std;

float32 worldWidth = 400;
int width = 800, height = 600;
const float renderScale = 10.f;
float scale = 0.2f;



b2Vec2 getMousePosition(sf::RenderWindow &window)
{
	sf::Vector2f mp = sf::Vector2f(sf::Mouse::getPosition(window));
	mp.x -= width / 2;
	mp.y -= height / 2;
	mp.x += window.getView().getCenter().x;
	mp.y += window.getView().getCenter().y;
	return b2Vec2(mp.x /  (renderScale / scale), mp.y / (renderScale / scale));
}

float randRange(float32 s, float32 e)
{
	return s + (e - s) * float(rand())/RAND_MAX;
}

bool elite = false;
int n_elite = 11;
int n_cars = n_elite * (n_elite + (elite ? 1 : -1)) / 2;
vector<Car*> cars;
sWorld world;
sConcavePolygon ground;

sPopulation population;

bool testForEnd()
{

	b2AABB endAABB;
	endAABB.lowerBound.Set(worldWidth - 5.f, -10);
	endAABB.upperBound.Set(worldWidth, 40);

	if(world.getBodiesAABB(endAABB).size()){
		return true;
	}
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

	float32 len = worldWidth;
	float32 h = 10.f + 50.f * float(rand())/RAND_MAX;
	ground.add(len,h + 10.f);
	ground.add(0.f,h + 10.f);
	ground.setPosition(-6, 5 - h * start_h);
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
}

int main()
{
	sf::Clock clck;

	srand(clck.getElapsedTime().asMicroseconds());

	State state(world.b2world);

	population.setElites(0);

	cars.resize(n_cars);
	for(int i=0; i<n_cars; i++){
		cars[i] = new Car;
		population.addLifeForm(cars[i]);
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



	bool isDragging = false;



	vector<b2Vec2> points;
	

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

	while(window.isOpen()){
		simFrame++;
		while(window.pollEvent(e)){
			if(e.type == sf::Event::Closed){
				window.close();
			} else if(e.type == sf::Event::MouseButtonPressed){
				if(e.mouseButton.button == sf::Mouse::Left){
					if(mouse_mode){
						if(!mouseDown){
							mouseJoint.pressed(getMousePosition(window));
							mouseDown = true;
						}
					} else {
						points.push_back(getMousePosition(window));
					}

				}
			} else if(e.type == sf::Event::MouseButtonReleased){
				//if(e.mouseButton.button == sf::Mouse::Left){
				//	mouseJoint.released();
				//}
			} else if(e.type == sf::Event::Resized){
				width = window.getSize().x;
				height = window.getSize().y;
				view.setSize(width, height);
			} else if(e.type == sf::Event::KeyPressed){
				if(e.key.code == sf::Keyboard::F5){
					state.save();
				} else if(e.key.code == sf::Keyboard::F6){
					state.apply();
				} else if(e.key.code == sf::Keyboard::F2){
					if(!mouse_mode){
						//  create shape
						sConcavePolygon *poly = new sConcavePolygon;
						poly->set(points);
						poly->setFriction(1);
						world.add(poly);

					} else {
						points.clear();
					}
					mouse_mode = !mouse_mode;
				} else if(e.key.code == sf::Keyboard::F){
					frameLimiter = !frameLimiter;
					window.setFramerateLimit(frameLimiter ? 60 : 2000);
					window.setVerticalSyncEnabled(frameLimiter);
				} else if(e.key.code == sf::Keyboard::R){
					render_flag = !render_flag;
				}
			}
		}
		bool step = true;


		if(mouseDown && !sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			mouseJoint.released();
			mouseDown = false;
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){

			//sRectangle *rec = new sRectangle(scale * float(rand()) / RAND_MAX + 0.1 , scale * float(rand()) / RAND_MAX + 0.1);//(0.1,0.1, 0,-5,1);
			//rec->setSize(0.1,0.1);
			//rec->setPosition(b2Vec2(0,0));
			//rec->setFriction(1.f);
			//world.add(rec);

		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::F7)){
			state.interpolate(0.5);
			//step = false;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
			for(int i=0; i<n_cars; i++){
				//cars[i]->setAccelerator(1);
			}

		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
			//car.setAccelerator(-1);
		} else {
			//car.setAccelerator(0);
		}
		mouseJoint.setPosition(getMousePosition(window));

		
		if(testForEnd()){
			simFrame = 0;


			population.newGeneration();
			for(int i = 0; i < population.size(); i++){
				world.remove(cars[i]);
				world.add(cars[i]);
			}
			createground();

		}
		if(simFrame == 120){
			for(int i = 0; i < n_cars; i++){
				cars[i]->setAccelerator(1);
			}
		}
		int t2 = clck.getElapsedTime().asMicroseconds();
		if(step)world.step();
		physicsTime += clck.getElapsedTime().asMicroseconds() - t2;
		

		

		t2 = clck.getElapsedTime().asMicroseconds();
		if(render_flag){


			
			if(t2 - render_t > 1000000.f / 60.f || frameLimiter){
				b2Vec2 wp = b2Vec2(float(view.getCenter().x) / 50.f, float(view.getCenter().y) / 50.f);
				b2Vec2 p(-100,0);
				for(int i=0;i <cars.size(); i++){
					if(cars[i]->alive){
						b2Vec2 cp = cars[i]->chassis.getPosition();
						if(p.x < cp.x){
							p = cp;
						}
					}
				}
				p.y = wp.y + 0.1f * (p.y - wp.y);
				view.setCenter(p.x * 50, p.y * 50);				
				window.setView(view);
				debugDraw.setView(view);

				window.clear();
				debugDraw.prepare();
				debugDraw.DrawDebugData(world);

				if(!mouse_mode){
					for(int i=1; i<points.size(); i++){
						debugDraw.DrawSegment(points[i-1], points[i], b2Color(1,1,0));
					}
					if(points.size())debugDraw.DrawSegment(points[points.size()-1], getMousePosition(window), b2Color(1,1,0));
				}

				debugDraw.finalize();

				window.display();
				renderTime += clck.getElapsedTime().asMicroseconds() - t2;

				render_t = t2;
			} else {
				//printf("skip render");
			}
		}

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

