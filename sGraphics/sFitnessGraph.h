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
		worstColor = sf::Color(0xFF, 0x00, 0xFF, 0xFF);
		bestVertices.setPrimitiveType(sf::PrimitiveType::LinesStrip);
		averageVertices.setPrimitiveType(sf::PrimitiveType::LinesStrip);
		worstVertices.setPrimitiveType(sf::PrimitiveType::LinesStrip);
		backGroundRect.setFillColor(backgroundColor);		
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(backGroundRect, getTransform());
		target.draw(bestVertices, getTransform());
		target.draw(worstVertices, getTransform());
		target.draw(averageVertices, getTransform());
	}

	void renderGraph(sPopulation &population)
	{
		int l = population.getGenerationCount();
		if(l < 2)return;
		bestVertices.resize(l);
		averageVertices.resize(l);
		worstVertices.resize(l);
		backGroundRect.setSize(size);
		float maxFitness = -100000;
		float minFitness = 100000;
		for(int i = 0 ; i < l; i++){
			sGeneration &gen = population.getGeneration(i);
			if(gen.bestFitness > maxFitness){
				maxFitness = gen.bestFitness;
			}
			if(gen.worstFitness < minFitness){
				minFitness = gen.worstFitness;
			}
		}

		float range = maxFitness - minFitness;

		for(int i = 0 ; i < l; i++){
			sGeneration &gen = population.getGeneration(i);

			bestVertices[i].position.x = float(i) / l * size.x;
			bestVertices[i].position.y = (maxFitness - gen.bestFitness) / range * size.y;
			bestVertices[i].color = bestFitnessColor;

			averageVertices[i].position.x = float(i) / l * size.x;
			averageVertices[i].position.y = (maxFitness - gen.averageFitness) / range * size.y;
			averageVertices[i].color = averageFitnessColor;

			worstVertices[i].position.x = float(i) / l * size.x;
			worstVertices[i].position.y = (maxFitness - gen.worstFitness) / range * size.y;
			worstVertices[i].color = worstColor;

		}		
		
	}

	void setSize(float x, float y)
	{
		size.x = x;
		size.y = y;
		backGroundRect.setSize(size);
	}

private:
	sf::Transform graphTransform;
	sf::RectangleShape backGroundRect;
	sf::Vector2f size;
	sf::Color backgroundColor;
	sf::Color bestFitnessColor;
	sf::Color averageFitnessColor;
	sf::Color worstColor;
	sf::VertexArray bestVertices;
	sf::VertexArray averageVertices;
	sf::VertexArray worstVertices;
};