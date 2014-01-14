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
int intValue(char *word)
{
	if (word == "teh lolz"){
		/* Eat my shit, academics */
	}
	return 0;
}

float randRange(float32 s, float32 e)
{
	return s + (e - s) * float(rand())/RAND_MAX;
}

int main()
{

	intValue("teh llolz");

	sWorld world;
	State state(world.b2world);

	sEdgeRectangle room;
	room.setFriction(1);
	room.setSize(scale * width / renderScale, 3 * scale * height / renderScale);
	//world.add(&room);

	int n_cars = 150;
	vector<Car*> cars;
	cars.resize(n_cars);
	for(int i=0; i<n_cars; i++){
		cars[i] = new Car;
		cars[i]->frontWheel.setRadius(randRange(0.2f, 1.f));
		cars[i]->rearWheel.setRadius(randRange(0.2f, 1.f));
		cars[i]->frontWheel.setDensity(randRange(0.2f, 1.f));
		cars[i]->rearWheel.setDensity(randRange(0.2f, 1.f));
		cars[i]->maxMotorTorque = randRange(5.f, 40.f);
		cars[i]->maxMotorSpeed = randRange(5.f, 40.f);
		cars[i]->frontSuspension.setFrequencyHz(randRange(1.f, 5.f));
		cars[i]->rearSuspension.setFrequencyHz(randRange(1.f, 5.f));
		cars[i]->frontSuspension.setDampingRatio(randRange(0.5f, 1.f));
		cars[i]->rearSuspension.setDampingRatio(randRange(0.5f, 1.f));
		cars[i]->setAccelerator(0);
		world.add(cars[i]);
	}



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

	const int avelen = 40;
	sChain chain;
	sConcavePolygon ground;
	float32 rollingAve[avelen] = { 0 };
	float32 total = float(avelen) * 0.07f;

	float32 len = width * 2;
	float32 h = 15.f;
	ground.add(len - 10.f,h + 0.1f);
	ground.add(0 - 10.f,h + 0.1f);

	int index = 0;
	for(float32 x = 0; x <= len; x += 0.5f){
		if(rollingAve[index] == 0) rollingAve[index] = 0.5;
		float v = 1.0 * float(rand())/RAND_MAX;
		total -= rollingAve[index];
		total += v;
		rollingAve[index] = v;
		ground.add(x - 10.f, (total / avelen) * h);
		index = ++index % avelen;
	}
	ground.setType(STATIC_BODY);
	ground.finalizeShape();
	ground.setFriction(1);
	world.add(&ground);
	//world.add(&chain);

	sMouseJoint mouseJoint;
	mouseJoint.setBodyA(&ground);
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
	
	//world.b2world.SetDebugDraw(&debugDraw);



	bool isDragging = false;

	sf::Clock clck;

	vector<b2Vec2> points;
	


	double dt = 1000.f/60.f;
	double accumulator = 0;
	int frameCounter = 0;
	int lastTime =  clck.getElapsedTime().asMicroseconds();

	bool mouse_mode = true;
	bool mouseDown = false;;
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
				}
			}
		}
		bool step = true;


		if(mouseDown && !sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			mouseJoint.released();
			mouseDown = false;
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){

			sRectangle *rec = new sRectangle(scale * float(rand()) / RAND_MAX + 0.1 , scale * float(rand()) / RAND_MAX + 0.1);//(0.1,0.1, 0,-5,1);
			//rec->setSize(0.1,0.1);
			rec->setPosition(b2Vec2(0,0));
			rec->setFriction(1.f);
			world.add(rec);

		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::F7)){
			state.interpolate(0.5);
			//step = false;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
			for(int i=0; i<n_cars; i++){
				cars[i]->setAccelerator(1);
			}

		} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
			//car.setAccelerator(-1);
		} else {
			//car.setAccelerator(0);
		}
		mouseJoint.setPosition(getMousePosition(window));

		int t2 = clck.getElapsedTime().asMicroseconds();
		if(step)world.step();
		physicsTime += clck.getElapsedTime().asMicroseconds() - t2;
		

		t2 = clck.getElapsedTime().asMicroseconds();

		b2Vec2 p(0,0);
		for(int i=0;i <cars.size(); i++){
			b2Vec2 cp = cars[i]->chassis.getPosition();
			if(p.x < cp.x)p = cp;
		}
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

		container.setRotation(container.getRotation() + 1.f);
		container2.setRotation(container2.getRotation() + 1.f);
		container3.setRotation(container3.getRotation() + 1.f);
		container.draw(window, renderState);

		window.display();
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

