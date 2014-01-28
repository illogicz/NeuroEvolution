#pragma once;
#include <sfml.h>
#include "../sEvolution/sPopulation.h"

using namespace sf;

class sNeuralNetDisplay : public Transformable
{


public:

	sNeuralNetDisplay()
	{
		setSize(250,250);
		backgroundColor = Color(0x7F, 0x7F, 0x7F, 0xFF);
		backGroundRect.setFillColor(backgroundColor);	
		padding = 13;
		neuronRadius = 7;
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
		
		// Draw Synapse Layers
		drawSynapseLayer(net.getInputNeurons(), 0);
		float l = net.getHiddenLayerCount();
		for(float i = 0; i < l; i += 1){
			drawSynapseLayer(net.getHiddenNeurons(i), i + 1);
		}

		// Draw Neurons
		drawNeuronLayer(net.getInputNeurons(), padding);
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
	float neuronRadius;

	void drawSynapseLayer(vector<sNeuron> &neurons, int layer)
	{
		VertexArray v;
		v.setPrimitiveType(PrimitiveType::Lines);
		int l1 = neurons.size();
		float maxw = neuralNet->getMaxWeight();
		for(int i = 0; i < l1; i++){
			Vector2f p1 = getNeuronPosition(layer, neurons[i].order, l1);
			int l2 = neurons[i].outputSynapses.size();
			float act = neurons[i].activation();
			for(int j = 0; j < l2; j++){

				int order = neurons[i].outputSynapses[j]->output->order;

				Vector2f p2 = getNeuronPosition(layer + 1, order, l2);

				float w = neurons[i].outputSynapses[j]->weight * act;
				//int c = (w / maxw + 1.f) * 0.5f * 0xFF;

				int a = abs(w / maxw) * 0xFF;
				int c = w < 0 ? 0x00 : 0xFF;

				v.append(Vertex(p1,Color(c, c, c, a)));
				v.append(Vertex(p2,Color(c, c, c, a)));
			}
		}
		
		target->draw(v, RenderStates(getTransform()));
	}

	void drawNeuronLayer(vector<sNeuron> &neurons, float y)
	{
		float l = neurons.size();
		for(int i = 0; i < l; i++){
			float x = (float(neurons[i].order) / (l - 1)) * (width - padding * 2) + padding;
			drawNeuron(neurons[i], x, y);
		}

	}

	Vector2f getNeuronPosition(int layer,  int index, int l)
	{
		float x = (float(index) / (l - 1)) * (width - padding * 2) + padding;
		float y = (float(layer) / (neuralNet->getHiddenLayerCount() + 1)) * (height - padding * 2) + padding;
		return Vector2f(x, y);
	}

	void drawNeuron(sNeuron &neuron, float x, float y)
	{
		neuronCircle.setPosition(x - neuronRadius, y - neuronRadius);
		neuronCircle.setRadius(neuronRadius);
		if(neuralNet->getMaxBias() > 0){
			neuronCircle.setOutlineThickness(3);
		} else {
			neuronCircle.setOutlineThickness(0);
		}
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