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
		feedbackRadius = 10;
		feedbackCircle.setOutlineThickness(2);
		feedbackCircle.setFillColor(Color::Transparent);
		feedbackCircle.scale(0.5,1);
		padding = 13;
		neuronRadius = 7;
		vertices.setPrimitiveType(PrimitiveType::Lines);
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

		neuronCircle.setRadius(neuronRadius);
		neuronCircle.setOutlineColor(sf::Color::Transparent);
		neuronCircle.setOutlineThickness(0);
		
		biasCircle.setRadius(neuronRadius + 2);
		biasCircle.setOutlineColor(sf::Color::Transparent);
		biasCircle.setOutlineThickness(0);




		vertices.clear();

		target->draw(backGroundRect, RenderStates(getTransform()));
	
		vector<sSynapse*> *synapses = &neuralNet->getSynapses();
		for(size_t i = 0; i < synapses->size(); i++){
			drawSynapse((*synapses)[i]);
		}
		target->draw(vertices, RenderStates(getTransform()));

		vector<sNeuron*> *neurons = &neuralNet->getNeurons();
		for(size_t i = 0; i < neurons->size(); i++){
			drawNeuron((*neurons)[i]);
		}


		
	}

	float neuronSize;
	float neuronSpacing;
	float layerSpacing;
	float padding;
	float neuronRadius;
	float feedbackRadius;

	void drawSynapse(sSynapse *synapse)
	{
		

		float w = synapse->weight* synapse->input->activation;
		int a = (int)(abs(w / neuralNet->getMaxWeight()) * 255.f);
		int c = w < 0 ? 0x00 : 0xFF;
		sf::Color color(c, c, c, a);

		if(synapse->input == synapse->output){  // feedback

			Vector2f p = getNeuronPosition(synapse->input);

			feedbackCircle.setRadius(feedbackRadius);
			feedbackCircle.setOutlineColor(color);
			feedbackCircle.setPosition(p.x, p.y - feedbackRadius);

			target->draw(feedbackCircle, RenderStates(getTransform()));

		} else if(synapse->input->biasNeuron){  // bias
				
			biasCircle.setFillColor(color);
			float r = biasCircle.getRadius();
			biasCircle.setPosition(getNeuronPosition(synapse->output) - Vector2f(r,r));
			target->draw(biasCircle, RenderStates(getTransform()));

		} else {                                // normal
				
			vertices.append(Vertex(getNeuronPosition(synapse->input),color));
			vertices.append(Vertex(getNeuronPosition(synapse->output),color));
		}

		
	}

	Vector2f getNeuronPosition(sNeuron *neuron)
	{
		float x;
		int l = neuralNet->getLayerSize(neuron->layer);
		if(l == 1){
			x = width / 2;
		} else {
			x = (float(neuron->order) / (l - 1)) * (width - padding * 2) + padding;
		}
		float y = (float(neuron->layer) / (neuralNet->getLayerCount() - 1)) * (height - padding * 2) + padding;
		return Vector2f(x, y);
	}

	void drawNeuron(sNeuron *neuron)
	{

		Vector2f p = getNeuronPosition(neuron);

		neuronCircle.setPosition(p.x - neuronRadius, p.y - neuronRadius);

		int oc = (int)((neuron->activation + 1.f) * 0.5f * 255.f);
		neuronCircle.setFillColor(Color(oc, oc, oc));

		target->draw(neuronCircle, RenderStates(getTransform()));



	}

	void drawSynapse(const sSynapse &neuron, int layer, int input_index, int output_index)
	{

	}

	float width;
	float height;

private:


	sf::VertexArray vertices;
	sNeuralNet *neuralNet;
	RenderTarget *target;
	RenderStates renderState;

	CircleShape neuronCircle;
	CircleShape biasCircle;
	CircleShape feedbackCircle;
	RectangleShape backGroundRect;
	Color backgroundColor;

};