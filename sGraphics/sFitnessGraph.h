#pragma once;
#include <sfml.h>
#include "../sEvolution/sPopulation.h"

class sFitnessGraph : public sf::Drawable, public sf::Transformable
{


public:

	sFitnessGraph()
	{
		size.x = 796;
		size.y = 40;
		backgroundColor = sf::Color(0x66, 0x66, 0x66, 0xFF);
		bestFitnessColor = sf::Color(0xFF, 0xFF, 0xFF, 0xFF);
		averageFitnessColor = sf::Color(0x00, 0xFF, 0xFF, 0xFF);
		bestVertices.setPrimitiveType(sf::PrimitiveType::LinesStrip);
		averageVertices.setPrimitiveType(sf::PrimitiveType::LinesStrip);
		backGroundRect.setFillColor(backgroundColor);
		backGroundRect.setSize(size);
		
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		states.transform *= getTransform();

		target.draw(backGroundRect, states);
		target.draw(bestVertices, states);
		target.draw(averageVertices, states);
	}

	void renderGraph(sPopulation &population)
	{
		int l = population.getGenerationCount();
		if(l < 2)return;
		bestVertices.resize(l);
		averageVertices.resize(l);
		float maxFitness = 0;
		for(int i = 0 ; i < l; i++){
			sGeneration &gen = population.getGeneration(i);
			if(gen.bestFitness > maxFitness){
				maxFitness = gen.bestFitness;
			}
		}
		for(int i = 0 ; i < l; i++){
			sGeneration &gen = population.getGeneration(i);

			bestVertices[i].position.x = float(i) / l * size.x;
			bestVertices[i].position.y = floor(size.y - gen.bestFitness / maxFitness * size.y) + 0.5f;
			bestVertices[i].color = bestFitnessColor;

			averageVertices[i].position.x = float(i) / l * size.x;
			averageVertices[i].position.y = floor(size.y - gen.averageFitness/ maxFitness * size.y) + 0.5f;
			averageVertices[i].color = averageFitnessColor;

		}
		
		
	}


private:

	sf::RectangleShape backGroundRect;
	sf::Vector2f size;
	sf::Color backgroundColor;
	sf::Color bestFitnessColor;
	sf::Color averageFitnessColor;
	sf::VertexArray bestVertices;
	sf::VertexArray averageVertices;
};