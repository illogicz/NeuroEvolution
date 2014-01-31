#pragma once
#include <sfml.h>
#include "sNeuralNet.h"
#include "..\sEvolution\sGenome.h"


class sNeuralNetworkAnalyser : public sf::Transformable
{
public:

	sNeuralNetworkAnalyser()
	{
		n_buckets = 200;
		dist.resize(n_buckets);
		va.setPrimitiveType(sf::PrimitiveType::Lines);
		graphWidth = 200;
		height = 180;
		scale = 20.f;
		num_samples = 100000;
		currentLayer = 0;
		inputRangeLower = -1;
		inputRangeUpper = 1;
	}

	float inputRangeLower;
	float inputRangeUpper;
	float maxWeight;
	float maxBias;
	float graphWidth;
	float width;
	float height;
	int num_samples;

	float getRandomWeight(float distribution)
	{
		float v = sRandom::getFloat(0,1);
		v = pow(v, distribution);
		if(sRandom::getBool()){
			v *= -1;
		}
		return v;
	}

	float getRandomBias(float distribution)
	{
		float v = sRandom::getFloat(0,1);
		v = pow(v, distribution);
		if(sRandom::getBool()){
			v *= -1;
		}
		return v;
	}

	void analyzeNetwork(sNeuralNet &neuralNet)
	{
		max_y = 0;
		float grandtotal = 0;
		scale = 100;
		width = graphWidth;

		for(int j = 0; j < num_samples; j++){

			float total = 0;
			neuralNet.randomize();
			neuralNet.prepare();
			for(int i = 0; i < neuralNet.getInputCount(); i++){
				neuralNet.setInput(i, sRandom::getFloat(inputRangeLower,inputRangeUpper));
			}
			neuralNet.run();
			total += neuralNet.getOutput(0);
			grandtotal += total;

			int bucket = (total * scale) + n_buckets / 2;

			if(bucket >= 0 && bucket < n_buckets){
				dist[bucket] += 1.f;
			}

		}
		for(int j = 0; j < n_buckets; j++){
			max_y = dist[j] > max_y ? dist[j] : max_y;
		}

		float ave_abs = grandtotal / float(num_samples) * scale + float(n_buckets / 2);
		sf::Color color(255,255,255);

		addLine(ave_abs,0,ave_abs,dist[int(ave_abs)], color);

		plotResult(color);
		plotAxis(1);

	}

	// Analyze layert output distributions
	void analyseLayers(sNeuralNet &neuralNet)
	{
		
		max_y = 0;
		int layers = neuralNet.getHiddenLayerCount() + 1;
		width = graphWidth * layers;


		for(int i = 0; i < layers; i++){


			float weightDist = neuralNet.getWeightDistribution(i);
			float biasDist = neuralNet.getBiasDistribution(i);

			sf::Color color(sRandom::getInt(50,0xFF), sRandom::getInt(50,0xFF), sRandom::getInt(50,0xFF));
			for(int i = 0; i < n_buckets; i++){
				dist[i] = 0;
			}
			int num_neurons;
			if(i == 0){
				num_neurons = neuralNet.getInputCount();
			} else {
				num_neurons = neuralNet.getHiddenLayerSize(i - 1);
			}
			float grandtotal = 0;
			for(int j = 0; j < num_samples; j++){
				float total = 0;
				for(int i = 0; i < num_neurons; i++){
					float input = sRandom::getFloat(inputRangeLower,inputRangeUpper);
					float bias = getRandomBias(biasDist) * neuralNet.getMaxBias();
					float weight = getRandomWeight(weightDist) * neuralNet.getMaxWeight();
					total += tanh_approx(input + bias) * weight;
				}
				grandtotal += abs(total);


				int bucket = (total * scale) + n_buckets / 2;

				if(bucket >= 0 && bucket < n_buckets){
					dist[bucket] += 1.f;
				}
			}

			for(int j = 0; j < n_buckets; j++){
				max_y = dist[j] > max_y ? dist[j] : max_y;
			}

			float ave_abs = grandtotal / float(num_samples) * scale + float(n_buckets / 2);

			addLine(ave_abs,0,ave_abs,dist[int(ave_abs)], color);

			plotResult(color);
			

			currentLayer++;
		}



		currentLayer = 0;
		for(int i = 0; i < layers; i++){
			plotAxis();
			currentLayer++;
		}
		
	}


	void draw(sf::RenderTarget &target)
	{

		sf::RectangleShape rect(sf::Vector2f(width,height));
		rect.setFillColor(sf::Color(30,30,30));
		target.draw(rect, getTransform());

		
		sf::Transform tf;
		tf.scale(float(graphWidth) / n_buckets, -height / max_y, 0,height - 1);//, height);
		//tf.scale(float(width) / n_buckets, height / max_y);
		target.draw(va, getTransform() * tf);
	}

	void plotAxis(float w = 3.f)
	{
		sf::Color color(0x7f, 0x7f, 0x7f);
		float x = n_buckets / 2 + w * scale;
		addLine(x, 0, x, max_y, color);
		x = n_buckets / 2 - w * scale;
		addLine(x, 0, x, max_y, color);
		addLine(n_buckets / 2, 0, n_buckets / 2, max_y, color);
	}

	void plotResult(sf::Color &color)
	{
		for(int j = 0; j < n_buckets - 1; j++){

			//printf("dist[j] = %f\n", dist[j]);
			addLine(float(j), dist[j], float(j + 1), dist[j + 1], color);

		}
	}

	void addLine(float x1, float y1, float x2, float y2, sf::Color &color)
	{
		sf::Vertex v;
		v.position.x = x1 + currentLayer * n_buckets;
		v.position.y = y1;
		v.color = color;
		va.append(v);
		v.position.x = x2 + currentLayer * n_buckets;
		v.position.y = y2;
		va.append(v);
	}

private:
	
	int currentLayer;
	float max_y;
	sf::VertexArray va;
	int n_buckets;
	float scale;
	vector<float> dist;
};