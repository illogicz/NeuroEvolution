#pragma once
#include <vector>
#include <algorithm>
#include "sPhenotype.h"

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
		m_generations = 0;
		selectionBias = 2.f;
		breadingPoolFraction = 1.0f;
		m_elites = 0.0f;
	}

	void addPhenotype(sPhenotype *phenotype)
	{
		m_phenotypes.push_back(phenotype);
	}


	void newGeneration()
	{
		// Sort based on fitness 
		sort(m_phenotypes.begin(), m_phenotypes.end(), sortPhenotypes);
		
		int valid_breaders = 1;

		// Save some data about last generation
		sGeneration generation;
		generation.bestGenome = m_phenotypes[0]->genome;
		generation.bestFitness = generation.averageFitness = m_phenotypes[0]->getFitness();
		for(int i = 1; i < m_phenotypes.size(); i++){
			if(m_phenotypes[i]->getFitness() > 0){
				valid_breaders++;
			}
			generation.averageFitness += m_phenotypes[i]->getFitness();
		}
		generation.averageFitness /= m_phenotypes.size();

		printf("\nGeneration %i\n", m_generations);
		printf("fitness best = %f average = %f\n", generation.bestFitness, generation.averageFitness);
		printf("uniformity = %f\n", generation.averageFitness / generation.bestFitness);
		//generation.bestGenome.print();
		m_generationHistory.push_back(generation);

		// Copy breader genomes
		vector<sGenome> breaders;
		int num_breaders = breadingPoolFraction * size();
		if(valid_breaders < num_breaders){
			printf("%i breaders infertile \n", num_breaders - valid_breaders);
			num_breaders = valid_breaders;
		}
		for(int i = 0; i < num_breaders; i++){
			breaders.push_back(m_phenotypes[i]->genome);
		}
		float n2 = pow(num_breaders, selectionBias);

		// Don't change the first ones, they become elites 
		for(int i = m_elites; i < m_phenotypes.size(); i++){

			// Choose 2 breaders, better performers have better chance, no cloning;

			int i1 = num_breaders - floor(pow(getRand(n2), 1.f / selectionBias)) - 1;
			int i2 = num_breaders - floor(pow(getRand(n2), 1.f / selectionBias)) - 1;
			while(i1 == i2){
				i2 = num_breaders - floor(pow(getRand(n2), 1.f / selectionBias)) - 1;
			}
			m_phenotypes[i]->genome.mate(breaders[i1], breaders[i2]);
		}

		m_generations++;
	}

	void setElites(int elites)
	{
		m_elites = elites;
	}
	int getElites()
	{
		return m_elites;
	}

	int getGenerationCount()
	{
		return m_generationHistory.size();
	}
	sGeneration& getGeneration(int index)
	{
		return m_generationHistory[index];
	}


	int size()
	{
		return m_phenotypes.size();
	}

	sPhenotype* operator[] (int x) {
          return m_phenotypes[x];
    }
	

private:

	float getRand(float max)
	{
		return max * float(rand()) / (RAND_MAX+1);
	}

	vector<int> sel_dist;

	int m_generations;
	vector<sGeneration> m_generationHistory;

	static bool sortPhenotypes(sPhenotype *lifeform1, sPhenotype *lifeform2)
	{
		return lifeform1->getFitness() > lifeform2->getFitness();
	}

	float selectionBias;
	float breadingPoolFraction;
	int m_elites;

	vector<sPhenotype*> m_phenotypes;


};



