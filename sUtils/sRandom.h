#pragma once
// mersenne_twister_engine constructor
#include <random>
#include <iostream>


using namespace std;


struct sRandom 
{
private:
	static mt19937 engine;
	static uniform_real_distribution<float> uniform_distribution;
	static normal_distribution<float> normal_distrubution;

public:
   
    static void seed(unsigned int new_seed = mt19937::default_seed) {
        engine.seed(new_seed);
    }
   
    static unsigned int get() {
        return engine();
    }

	static float getFloat(float min, float max)
	{
		return min + (max - min) * uniform_distribution(engine);
		//return float_distribution(engine);
	}	
	
	static int getInt(int min, int max)
	{
		return min + int(floor(float(max - min + 1) * uniform_distribution(engine)));
	}

	static bool getBool()
	{
		return uniform_distribution(engine) < 0.5f;
	}

	static float getNormal(float mean, float stddev)
	{
		return normal_distrubution(engine) * stddev + mean;
	}


	void test()
	{
		int l = 10000000;
		int ti = 0;
		float tf = 0;
		for(int i=0; i < l; i++){
			float f = sRandom::getFloat(0,1);
			tf += f;
			if(f <= 0 || f >= 1){
				printf("error %f \n", f);
			}
			int j = sRandom::getInt(0,1);
			ti += j;
			if(j < 0 || j > 1){
				printf("int error %i \n", j);
			}		
		}
		printf("total %f \n", tf);
		printf("total %i \n", ti);
	}

};

std::mt19937 sRandom::engine;
uniform_real_distribution<float> sRandom::uniform_distribution(0.f,0.9999999f);
normal_distribution<float> sRandom::normal_distrubution(0.f,1.f);