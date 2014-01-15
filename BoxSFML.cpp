// BoxSFML.cpp : Defines the entry point for the console application.
#include <sfml.h>
#include <Box2D\Box2D.h>
#include <vector>
#include <algorithm>
#include "State.h"
#include "sPhysics\sPhysics.h"
#include "sGraphics\sDebugDraw.h"
#include "sGraphics\sDisplayContainer.h"

#include "Car.h"

#include <random>

using namespace std;

float32 worldWidth = 600;
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

int n_elite = 10;
int n_cars = n_elite * (n_elite + 1) / 2;
vector<Car*> cars;
sWorld world;
sConcavePolygon ground;

bool testForEnd()
{

	b2AABB endAABB;
	endAABB.lowerBound.Set(worldWidth - 3.f, -10);
	endAABB.upperBound.Set(worldWidth, 40);

	if(world.getBodiesAABB(endAABB).size()){
		printf("END\n");
		return true;
	}
	for(int i = 0; i < n_cars; i++){
		if(cars[i]->alive) return false;
	}
	printf("END:  all dead\n");
	return true;
}


bool sortCars(Car* car1, Car *car2)
{
	return car1->chassis.getPosition().x  > car2->chassis.getPosition().x;
}

void createground()
{
	if(ground.isInWorld()){
		world.remove(&ground);
		ground.resetShape();
	}
	const int avelen = 40;

	float32 rollingAve[avelen] = { 0 };
	for(int i=0;i <avelen; i++){
		rollingAve[i] = 0.5f;
	}
	float32 total = float(avelen) * 0.5f;

	float32 len = worldWidth;
	float32 h = 40.f;
	ground.add(len,h + 0.1f);
	ground.add(0.f,h + 0.1f);
	ground.setPosition(-4, 4 - h / 2);
	int index = 0;
	for(float32 x = 0; x <= len; x += 0.5f){
		if(rollingAve[index] == 0) rollingAve[index] = 0.5;
		float v = 1.0 * float(rand())/RAND_MAX;
		total -= rollingAve[index];
		total += v;
		rollingAve[index] = v;
		ground.add(x, (total / avelen) * h);
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

	sEdgeRectangle room;
	room.setFriction(1);
	room.setSize(scale * width / renderScale, 3 * scale * height / renderScale);
	//world.add(&room);

	cars.resize(n_cars);
	for(int i=0; i<n_cars; i++){
		cars[i] = new Car;
		cars[i]->randomize();
		world.add(cars[i]);
	}





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
			sort(cars.begin(), cars.end(), sortCars);
			int car_index = n_elite;
			for(int i = 0; i < n_elite; i++){
				world.remove(cars[i]);
				world.add(cars[i]);
				for(int j = i + 1; j < n_elite; j++){
					world.remove(cars[car_index]);
					cars[car_index]->mate(cars[i], cars[j]);
					world.add(cars[car_index]);
					car_index++;
				}
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

		b2Vec2 p(-100,0);
		for(int i=0;i <cars.size(); i++){
			if(cars[i]->alive){
				b2Vec2 cp = cars[i]->chassis.getPosition();
				if(p.x < cp.x)p = cp;
			}
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

