#pragma once
#include <sfml.h>
#include "sText.h"
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
		m_geneNames.resize(width);
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(m_sprite, states);
		sf::Vector2i mp(getInverseTransform().transformPoint(mousePosition));
		//printf("%i %i\n", mp.x , mp.y);
		if(mp.x >= 0 && mp.y >= 0 && mp.x < width && mp.y < height){
			string geneName = m_geneNames[mp.x];
			sText::drawText(target, geneName, mousePosition.x, getPosition().y - 20, 18);
		}
	}

	void plot()
	{

		for(size_t i = 0; i < m_population->size(); i++){
			map<string, sGene>& genes = (*m_population)[i]->genome.getGenes();
			int x = 0;
			for(map<string, sGene>::iterator j = genes.begin(); j != genes.end(); ++j){
				int c = (j->second.getBinaryValue() >> 8) & 0xFF;
				m_image.setPixel(x, i, sf::Color(c,c,c));
				if(i == 0)m_geneNames[x] = j->first;
				x++;
			}
		}
		m_texture.loadFromImage(m_image);

	}



	int width;
	int height;
	sf::Vector2f mousePosition;

private:


	void drawTooltip()
	{




	}

	sPopulation *m_population;

	vector<string> m_geneNames;
	sf::Image m_image;
	sf::Sprite m_sprite;
	sf::Texture m_texture;
};