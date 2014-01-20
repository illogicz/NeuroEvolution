#pragma once
#include <vector>
#include <algorithm>
#include "sPhenotype.h"

struct sGeneration
{
	float bestFitness;
	float worstFitness;
	float averageFitness;
	sGenome bestGenome;
};

class sPopulation
{
public:

	sPopulation()
	{
		minBreaders = 3;
		m_generations = 0;
		m_selectionBias = 2.f;
		m_mutationRate = 0.01f;
		m_breadingPoolFraction = 1.0f;
		m_elites = 0.0f;
	}

	void addPhenotype(sPhenotype *phenotype)
	{
		m_phenotypes.push_back(phenotype);
		breadingDistribution.push_back(0);
	}



	// Generates a new generation of genome
	void newGeneration()
	{
		// Sort based on fitness 
		sort(m_phenotypes.begin(), m_phenotypes.end(), sortPhenotypes);
		
		int valid_breaders = 1;

		// Get data about last generation
		sGeneration generation;
		generation.bestGenome = m_phenotypes[0]->genome;
		generation.bestFitness = generation.averageFitness = m_phenotypes[0]->getFitness();
		generation.worstFitness = m_phenotypes[int(m_phenotypes.size() * 0.75f)]->getFitness();
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
		int num_breaders = m_breadingPoolFraction * size();
		if(valid_breaders < num_breaders){
			num_breaders = valid_breaders;
		}
		if(num_breaders < minBreaders)num_breaders = minBreaders;
		for(int i = 0; i < num_breaders; i++){
			breaders.push_back(m_phenotypes[i]->genome);
		}
		
		
		// Breading selection range, selection bias 1.0 - 3.0, default 2.0
		float n2 = pow(num_breaders, m_selectionBias);
		

		// Don't change the first ones, they become elites 
		for(int i = m_elites; i < m_phenotypes.size(); i++){

			// Choose 2 breaders, better performers have better chance, no cloning;

			int i1 = num_breaders - floor(pow(getRand(n2), 1.f / m_selectionBias)) - 1;
			int i2 = num_breaders - floor(pow(getRand(n2), 1.f / m_selectionBias)) - 1;
			while(i1 == i2){
				i2 = num_breaders - floor(pow(getRand(n2), 1.f / m_selectionBias)) - 1;
			}
			breadingDistribution[i1]++;
			breadingDistribution[i2]++;
			m_phenotypes[i]->genome.mate(breaders[i1], breaders[i2], m_mutationRate);
		}
		printBreadingDistribution();
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

	void setSelectionBias(float selectionBias)
	{
		m_selectionBias = selectionBias;
	}

	void setMutationRate(float mutationRate)
	{
		m_mutationRate = mutationRate;
	}

	void setBreadingPoolFraction(float breadingPoolFraction)
	{
		m_breadingPoolFraction = breadingPoolFraction;
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
	

	int minBreaders;

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

	float m_mutationRate;
	float m_selectionBias;
	float m_breadingPoolFraction;
	int m_elites;



	
	vector<sPhenotype*> m_phenotypes;

	vector<int> breadingDistribution;
	void printBreadingDistribution()
	{
		for(int i = 0; i < size(); i++){
			printf("%i : %i\n", i, breadingDistribution[i]);
		}
	}


};



