// BoxSFML.cpp : Defines the entry point for the console application.
#include <sfml.h>
#include <Box2D\Box2D.h>
#include <vector>
#include "State.h"
#include "sPhysics\sPhysics.h"
#include "sGraphics\sDebugDraw.h"
#include "Car.h"

#include <random>

using namespace std;

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

int main()
{



	sWorld world;
	State state(world.b2world);

	sEdgeRectangle room;
	room.setSize(scale * width / renderScale, scale * height / renderScale);
	world.add(&room);

	Car car;
	world.add(&car);

	sMouseJoint mouseJoint;
	mouseJoint.setBodyA(&room);
	world.add(&mouseJoint);

	srand(time(NULL));


	sf::Event e;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(width, height), "SFML works!", sf::Style::Default, settings);
	sf::View view;
	view.setSize(width, height);
	view.setCenter(0.5f, -0.5f);
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	
	
	sDebugDraw debugDraw(window);

	sf::Transform trans;
	//trans.translate(0.5f, -150.f);
	trans.scale(renderScale/scale, renderScale/scale);

	debugDraw.setTransform(trans);
	debugDraw.SetFlags(sDebugDraw::e_shapeBit | sDebugDraw::e_jointBit);
	
	world.b2world.SetDebugDraw(&debugDraw);



	bool isDragging = false;

	sf::Clock clck;

	vector<b2Vec2> points;
	
	double dt = 1000.f/60.f;
	double accumulator = 0;
	int frameCounter = 0;
	int lastTime =  clck.getElapsedTime().asMilliseconds();

	bool mouse_mode = true;

	int renderTime = 0;
	int physicsTime = 0;
	int oldt2 = lastTime;
	while(window.isOpen()){

		while(window.pollEvent(e)){
			if(e.type == sf::Event::Closed){
				window.close();
			} else if(e.type == sf::Event::MouseButtonPressed){
				if(e.mouseButton.button == sf::Mouse::Left){
					if(mouse_mode){
						mouseJoint.pressed(getMousePosition(window));
					} else {
						points.push_back(getMousePosition(window));
					}

				}
			} else if(e.type == sf::Event::MouseButtonReleased){
				if(e.mouseButton.button == sf::Mouse::Left){
					mouseJoint.released();
				}
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
						world.add(poly);

					} else {
						points.clear();
					}
					mouse_mode = !mouse_mode;
				}
			}
		}
		bool step = true;
		window.setView(view);
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){

			sRectangle *rec = new sRectangle(scale * float(rand()) / RAND_MAX + 0.1 , scale * float(rand()) / RAND_MAX + 0.1);//(0.1,0.1, 0,-5,1);
			//rec->setSize(0.1,0.1);
			rec->setPosition(b2Vec2(0,0));
			world.add(rec);

		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::F7)){
			state.interpolate(0.5);
			//step = false;
		}

		mouseJoint.setPosition(getMousePosition(window));

		int t2 = clck.getElapsedTime().asMilliseconds();
		//accumulator += t2 - oldt2;
		//oldt2 = t2;
		//if(accumulator >= dt){
		//	world.Step(dt / 1000.f, velocityIterations, positionIterations);
		//	accumulator -= dt;
		//}
		if(step)world.step();

		window.clear();
		debugDraw.prepare();
		world.b2world.DrawDebugData();
		debugDraw.finalize();
		window.display();

		frameCounter++;
		t2 = clck.getElapsedTime().asMilliseconds();
		if(t2 - lastTime > 999){
			frameCounter = 0;
			lastTime = t2;
			physicsTime = renderTime = 0;
		}

	}



	return 0;
}

