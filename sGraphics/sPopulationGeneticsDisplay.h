#pragma once
#include <sfml.h>
#include "../sEvolution/sGenome.h"
#include "../sEvolution//sPopulation.h"

class sPopulationGeneticsDisplay : public sf::Drawable, public sf::Transformable
{

public:

	void init(sPopulation &population)
	{
		m_population = &population;
		width = population[0]->genome.size();
		height = population.size();
		m_image.create(width,height);
		m_texture.create(width, height);
		m_texture.loadFromImage(m_image);
		m_sprite.setTexture(m_texture);
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(m_sprite, states);
	}

	void plot()
	{

		for(int i = 0; i < m_population->size(); i++){
			map<string, sGene>& genes = (*m_population)[i]->genome.getGenes();			
			int x = 0;
			for(map<string, sGene>::iterator j = genes.begin(); j != genes.end(); ++j){
				int c = (j->second.getBinaryValue() >> 8) & 0xFF;
				m_image.setPixel(x++, i, sf::Color(c,c,c));
			}
		}
		m_texture.loadFromImage(m_image);
	}

	int width;
	int height;

private:

	sPopulation *m_population;

	sf::Image m_image;
	sf::Sprite m_sprite;
	sf::Texture m_texture;
};