#pragma once
#include <vector>
#include <algorithm>
#include "sLifeform.h"

struct sGeneration
{
	float bestFitness;
	float averageFitness;
	sGenome bestGenome;
};

class sPopulation
{
public:

	sPopulation()
	{
		selectionBias = 1.5f;
		breadingPoolFraction = 1.0f;
		m_elites = 0.0f;
	}

	void addLifeForm(sLifeForm *lifeForm)
	{
		m_lifeForms.push_back(lifeForm);
	}


	void newGeneration()
	{
		// Sort based on fitness 
		sort(m_lifeForms.begin(), m_lifeForms.end(), sortLifeForms);
		
		int valid_breaders = 1;

		sel_dist.resize(m_lifeForms.size());


		// Save some data about last generation
		sGeneration generation;
		generation.bestGenome = m_lifeForms[0]->genome;
		generation.bestFitness = generation.averageFitness = m_lifeForms[0]->getFitness();
		for(int i = 1; i < m_lifeForms.size(); i++){
			if(m_lifeForms[i]->getFitness() > 0){
				valid_breaders++;
			}
			generation.averageFitness += m_lifeForms[i]->getFitness();
		}
		generation.averageFitness /= m_lifeForms.size();

		printf("\nGeneration %i\n", m_generations);
		printf("fitness best = %f average = %f\n", generation.bestFitness, generation.averageFitness);
		printf("uniformity = %f\n", generation.averageFitness / generation.bestFitness);
		generation.bestGenome.print();
		m_generationHistory.push_back(generation);

		// Copy breader genomes
		vector<sGenome> breaders;
		int num_breaders = breadingPoolFraction * size();
		if(valid_breaders < num_breaders){
			printf("%i breaders infertile \n", num_breaders - valid_breaders);
			num_breaders = valid_breaders;
		}
		for(int i = 0; i < num_breaders; i++){
			breaders.push_back(m_lifeForms[i]->genome);
		}
		float n2 = pow(num_breaders, selectionBias);

		// Don't change the first ones, they become elites 
		for(int i = m_elites; i < m_lifeForms.size(); i++){

			// Choose 2 breaders, better performers have better chance, no cloning;

			int i1 = num_breaders - floor(pow(getRand(n2), 1.f / selectionBias)) - 1;
			int i2 = num_breaders - floor(pow(getRand(n2), 1.f / selectionBias)) - 1;
			while(i1 == i2){
				i2 = num_breaders - floor(pow(getRand(n2), 1.f / selectionBias)) - 1;
			}
			sel_dist[i1]++;
			sel_dist[i2]++;
			m_lifeForms[i]->genome.mate(breaders[i1], breaders[i2]);
		}

		for(int i=0; i < m_lifeForms.size(); i++){
			printf("%i %i\n", i, sel_dist[i]);
		}

		m_generations++;
	}

	void setElites(int elites)
	{
		m_elites = elites;
	}
	
	int size()
	{
		return m_lifeForms.size();
	}

private:

	float getRand(float max)
	{
		return max * float(rand()) / (RAND_MAX+1);
	}

	vector<int> sel_dist;

	int m_generations;
	vector<sGeneration> m_generationHistory;

	static bool sortLifeForms(sLifeForm *lifeform1, sLifeForm *lifeform2)
	{
		return lifeform1->getFitness() > lifeform2->getFitness();
	}

	float selectionBias;
	float breadingPoolFraction;
	int m_elites;

	vector<sLifeForm*> m_lifeForms;


};



