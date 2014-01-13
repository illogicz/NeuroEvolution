// BoxSFML.cpp : Defines the entry point for the console application.
#include <sfml.h>
#include <Box2D\Box2D.h>
#include <vector>
#include "State.h"
#include "sPhysics\sPhysics.h"
#include "sGraphics\sDebugDraw.h"
#include "sGraphics\sDisplayContainer.h"

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
	//world.add(&room);

	Car car;
	world.add(&car);



	srand(time(NULL));


	sDisplayContainer container, container2, container3;
	container.setPosition(0,0);
	container2.setPosition(-100,0);
	container3.setPosition(100,0);

	sDisplayObject circle1, circle2, circle3, circle4;
	circle1.setPosition(-30, 0);
	circle2.setPosition(30, 0);
	circle3.setPosition(-30, 0);
	circle4.setPosition(30, 0);



	container.addChild(&container2);
	container.addChild(&container3);

	container2.addChild(&circle1);
	container2.addChild(&circle2);
	container3.addChild(&circle3);
	container3.addChild(&circle4);

	sChain chain;
	for(float32 x = -width/100; x <= width/10; x += 1.f){
		chain.addVertex(x, 1.0 * float(rand())/RAND_MAX + 3.f);
	}
	chain.setFriction(1);
	world.add(&chain);

	sMouseJoint mouseJoint;
	mouseJoint.setBodyA(&chain);
	world.add(&mouseJoint);


	sf::Event e;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(width, height), "SFML works!", sf::Style::Default, settings);
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
		b2Vec2 p = car.m_car.getPosition();
		view.setCenter(p.x * 50, p.y * 50);
		window.setView(view);
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){

			sRectangle *rec = new sRectangle(scale * float(rand()) / RAND_MAX + 0.1 , scale * float(rand()) / RAND_MAX + 0.1);//(0.1,0.1, 0,-5,1);
			//rec->setSize(0.1,0.1);
			rec->setPosition(b2Vec2(0,0));
			world.add(rec);

		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::F7)){
			state.interpolate(0.5);
			//step = false;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
			car.setAccelerator(1);
		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
			car.setAccelerator(-1);
		} else {
			car.setAccelerator(0);
		}
		mouseJoint.setPosition(getMousePosition(window));

		int t2 = clck.getElapsedTime().asMilliseconds();
		if(step)world.step();
		physicsTime += clck.getElapsedTime().asMilliseconds() - t2;
		window.clear();

		t2 = clck.getElapsedTime().asMilliseconds();
		debugDraw.prepare();
		world.b2world.DrawDebugData();
		debugDraw.finalize();

		container.setRotation(container.getRotation() + 1.f);
		container2.setRotation(container2.getRotation() + 1.f);
		container3.setRotation(container3.getRotation() + 1.f);
		container.draw(window, renderState);

		window.display();
		renderTime += clck.getElapsedTime().asMilliseconds() - t2;

		

		frameCounter++;
		t2 = clck.getElapsedTime().asMilliseconds();
		if(t2 - lastTime > 999){
			window.setTitle(to_string(frameCounter) + "fps " + to_string(renderTime) + ":" + to_string(physicsTime));
			frameCounter = 0;
			lastTime = t2;
			physicsTime = renderTime = 0;
		}

	}



	return 0;
}

