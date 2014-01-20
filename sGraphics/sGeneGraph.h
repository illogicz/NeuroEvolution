#pragma once
#include <sfml.h>
#include "../sEvolution/sGenome.h"

class sGeneGraph : public sf::Drawable, public sf::Transformable
{

public:

	sGeneGraph()
	{
		fillColor = sf::Color::White;
		backgroundColor = sf::Color(0x66, 0x66, 0x66, 0xFF);
		m_width = 40;
		m_height = 40;
		m_image.create(m_width,m_height);
		m_texture.create(m_width, m_height);
		m_texture.loadFromImage(m_image);
		m_sprite.setTexture(m_texture);
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		target.draw(m_sprite, states);
	}

	void plotGene(sPopulation &population, const string &geneType)
	{
		clear();
		vector<int> buckets(m_width,0);

		for(int i = 0; i < population.size(); i++){
			float v = population[i]->genome.getGene(geneType).getNormalizedValue();
			int b = v * m_width;
			buckets[b]++;
		}
		int max = 0;
		for(int i=0 ; i < m_width; i++){
			if(buckets[i] > max) max = buckets[i];
		}
		for(int i = 0 ; i < m_width; i++){
			int h = ceil(m_height * buckets[i] / max);
			for(int y = 0; y < h; y++){
				m_image.setPixel(i, m_height - y - 1, fillColor);
			}
		}
		m_texture.loadFromImage(m_image);
	
	}

	void plotGene2D(sPopulation &population, const string &geneType1, const string &geneType2)
	{
		float v1, v2;
		clear();
		for(int i = 0; i < population.size(); i++){
			v1 = population[i]->genome.getGene(geneType1).getNormalizedValue();
			int x = v1 * m_width;
			if(x == m_width)x--;
			v2 = population[i]->genome.getGene(geneType2).getNormalizedValue();
			int y = v2 * m_height;
			if(y == m_height)y--;

			m_image.setPixel(x, m_height - y - 1, fillColor);
		}
		m_texture.update(m_image);
	}

private:

	void clear()
	{
		for(int x = 0 ; x < m_width; x++){
			for(int y = 0; y < m_height; y++){
				m_image.setPixel(x, y, backgroundColor);
			}
		}
	}

	sf::Color fillColor;
	sf::Color backgroundColor;
	int m_width;
	int m_height;
	sf::Image m_image;
	sf::Sprite m_sprite;
	sf::Texture m_texture;
};