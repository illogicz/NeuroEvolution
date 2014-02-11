#pragma once
#include "../../sEvolution/sSimulation.h"
#include "../../sUtils/perlin.h"



class sRaceFitness : public sFitnessFunction
{
public: 
	sRaceFitness() : useSpeed(false), absolueDistance(false) {}
	bool useSpeed;
	bool absolueDistance;
	 float operator()(sPhenotype *phenotype)
	{
		float distance = phenotype->getPosition().x;
		if(distance < 0){
			distance = 0;
		}
		float energy = ((Worm*)phenotype)->getEneryUsed() + 2;
		float speed = abs(distance) / float(phenotype->lifeTime + 1);

		if(useSpeed){
			//return distance * speed;
			return 100.f * speed * distance;
		} else {
			return distance;
		}
	}
};

template <typename PhenotypeClass>
class RaceSimulation : public sSimulation
{

public:

	RaceSimulation()
	{
		(void)static_cast<sPhenotype*>((PhenotypeClass*)0);

		worldWidth = 600;
		worldOffset = 6;
		worldOffset_y = 5;
		populationSize = 50;
		randomizeEnvironment = true;
		minRoughness = 4;
		maxRoughness = 4;
		groundSegmentSize = 0.5f;
		groundType = GroundType::PerlinNoise;
		perlinOctaves = 5;
		perlinFrequency = 8;
		groundFrequency = 2.f;
		firstStepHeight = 0.1;
		stepIncrease = 0.05f;
		groundRampup = 50;
		ground.vertexSplitBaseWeight = 1.f;
		ground.doubleSolveWeight = 2.f;
		ground.segmentSplitBaseWeight = 1.f;
		ground.lengthWeight = 0.5f;
		ground.dotWeight = 1.f;

		worlds[0].setGroundBody(&ground);

		fitnessFunction.useSpeed = true;

	}

	~RaceSimulation()
	{
		for(int i = 0; i < populationSize; i++){
			delete population[i];
		}
	}



	// Simulation Settings
	float worldWidth;
	float worldOffset;
	float worldOffset_y;
	int populationSize;
	bool randomizeEnvironment;

	static enum GroundType	{ PerlinNoise, SinWaves, Steps };
	GroundType groundType;
	float minRoughness;
	float maxRoughness;
	float perlinFrequency;
	float perlinOctaves;
	float groundFrequency;
	float groundSegmentSize;
	float groundRampup;
	float firstStepHeight;
	float stepIncrease;
	sRaceFitness fitnessFunction;

protected:

	void initPhenotypes()
	{
		for(int i = 0; i < populationSize; i++){
			sPhenotype *phenotype = static_cast<sPhenotype*>(new PhenotypeClass);
			phenotype->fitnessFunction = &fitnessFunction;
			addPhenotype(phenotype);
		}
	}


	// Return whether the simulation is finished
	// In this case it's when all phenotypes are no longer active/alive
	bool isFinished()
	{
		for(int i = 0; i < population.size(); i++){
			if(population[i]->getPosition().x > worldWidth - worldOffset){
				population[i]->die();
				return true;
			}
		}
		
		// Speed up simulation if leader is not alive
		//if(leader != nullptr)speedUp = !leader->alive;



		for(int i = 0; i < population.size(); i++){
			if(population[i]->alive) return false;
		}
		return true;
	}

	// Build / Rebuild environment
	// Builds the ground based on roughness inputs
	void buildEnvironment()
	{
		if(ground.isInWorld()){
			// Don't regenrate if we don't randomize
			if(!randomizeEnvironment)return;

			// Remove and reset
			worlds[0].remove(&ground);
			ground.resetShape();
		}

		Perlin perlin(perlinOctaves, perlinFrequency, 0.5, sRandom::getInt(0,100000));


		randOffset1 = sRandom::getFloat(0,worldWidth);
		randOffset2 = sRandom::getFloat(0,worldWidth);
		randOffset3 = sRandom::getFloat(0,worldWidth);



		float h = sRandom::getFloat(minRoughness, maxRoughness);
		float start_h;
		if(groundType != GroundType::Steps){
			start_h = getHeightValue(worldOffset, perlin) * h;
		} else {
			start_h = 5;
		}
		ground.add(worldWidth, h + 1.f);
		ground.add(-2.f, h + 1.f);
		ground.add(-2.f, start_h - 3);
		ground.add(-1.f, start_h - 3);
		ground.add(-1.f, start_h);
		ground.add(worldOffset, start_h);
		ground.setPosition(3.f - worldOffset, worldOffset_y - start_h);
		int index = 0;
		float lasth = start_h;
		float stepHeight = firstStepHeight;
		printf("%f \n", groundSegmentSize);
		for(float x = worldOffset; x <= worldWidth; x += groundSegmentSize){

			if(groundType == GroundType::Steps){
				//printf("h = %f \n", start_h + lastx * lastx * stepheightf);
				ground.add(x, lasth);
				lasth -= stepHeight;
				stepHeight += stepIncrease;
				if(x < worldWidth - groundSegmentSize){
					//ground.add(x, lasth);
				}
			} else {
				float y = getHeightValue(x, perlin) * h;
				if(x < groundRampup){
					y = start_h + (y - start_h) * (x / groundRampup);
				}
				ground.add(x, y);
			}
		}
		ground.finalizeShape();
		ground.setType(STATIC_BODY);
		ground.setFriction(1);
		ground.setRestitution(0);
		worlds[0].add(&ground);

		speedUp = false;
	}


private:

	// returns a value between 0 - 1;
	float getHeightValue(float x, Perlin &perlin)
	{
		x *= groundFrequency;
		float v;
		if(groundType == GroundType::PerlinNoise){
			v = perlin.Get(x / worldWidth, 0);
		} else if(groundType == GroundType::SinWaves){
			float w1 = sin(randOffset1 + x / worldWidth * 59.f) * 0.25f;
			float w2 = sin(randOffset2 + x / worldWidth * 231.f) * 0.15f;
			float w3 = sin(randOffset3 + x / worldWidth * 851.f) * 0.05f;
			v = w1 + w2 + w3;
		} else {

		}
		return 0.5f + v;
	}
	float randOffset1, randOffset2, randOffset3;

	sConcavePolygon ground;
};