#pragma once;
#include <sfml.h>
#include "../sEvolution/sPopulation.h"

using namespace sf;

class sNeuralNetDisplay : public Transformable
{


public:

	sNeuralNetDisplay()
	{
		setSize(200,200);
		backgroundColor = Color(0x7F, 0x7F, 0x7F, 0xFF);
		backGroundRect.setFillColor(backgroundColor);	
		padding = 15;
	}

	void setSize(float w, float h)
	{
		width = w;
		height = h;
		backGroundRect.setSize(Vector2f(w, h));
	}


	void renderNeuralNet(RenderTarget &tar, sNeuralNet &net)
	{
		neuralNet = &net;
		target = &tar;

		target->draw(backGroundRect, RenderStates(getTransform()));

		drawNeuronLayer(net.getInputNeurons(), padding);

		float l = net.getHiddenLayerCount();
		for(float i = 0; i < l; i += 1){
			float y = ((i + 1) / (l + 1)) * (height - padding * 2) + padding;
			drawNeuronLayer(net.getHiddenNeurons(i), y);
		}

		drawNeuronLayer(net.getOutputNeurons(), height - padding);

	}

	float neuronSize;
	float neuronSpacing;
	float layerSpacing;
	float padding;

	void drawNeuronLayer(vector<sNeuron> &neurons, float y)
	{
		float l = neurons.size();
		for(int i = 0; i < l; i++){
			float x = (float(i) / (l - 1)) * (width - padding * 2) + padding;
			drawNeuron(neurons[i], x, y);
		}

	}

	void drawNeuron(sNeuron &neuron, float x, float y)
	{
		neuronCircle.setPosition(x - 10, y - 10);
		neuronCircle.setRadius(20);
		neuronCircle.setOutlineThickness(4);
		
		int oc;
		if(neuralNet->getMaxBias()){
			oc = (((neuron.bias / neuralNet->getMaxBias()) + 1.f) * 0.5f) * 0xFF;
		} else {
			oc = 0x7F;
		}
		neuronCircle.setOutlineColor(Color(oc, oc, oc));

		oc = ((neuron.activation() + 1.f) * 0.5f) * 0xFF;
		neuronCircle.setFillColor(Color(oc, oc, oc));

		target->draw(neuronCircle, RenderStates(getTransform()));

	}

	void drawSynapse(const sSynapse &neuron, int layer, int input_index, int output_index)
	{

	}

	float width;
	float height;

private:


	sNeuralNet *neuralNet;
	RenderTarget *target;
	RenderStates renderState;

	CircleShape neuronCircle;
	RectangleShape backGroundRect;
	Color backgroundColor;

};