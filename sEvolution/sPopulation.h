// TODO: Add some selection methods
// Determanistic

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
		minBreaders = 2;
		m_generations = 0;
		m_selectionBias = 2.f;
		m_mutationRate = 0.01f;
		m_breadingPoolFraction = 1.0f;
		m_elites = 0;
	}

	void addPhenotype(sPhenotype *phenotype)
	{
		m_phenotypes.push_back(phenotype);
		phenotype->setIsElite(false);
		breadingDistribution.push_back(0);
	}


	// Sort based on fitness 
	void sortPhenotypes()
	{
		sort(m_phenotypes.begin(), m_phenotypes.end(), sortPhenotypesFunction);
	}

	// Generates a new generation of genome
	void newGeneration()
	{
		
		sortPhenotypes();
		
		int valid_breaders = 1;

		// Store data about last generation
		sGeneration generation;

		// Store best performer genome
		generation.bestGenome = m_phenotypes[0]->genome;
		generation.bestFitness = generation.averageFitness = m_phenotypes[0]->getFitness();
		generation.worstFitness = m_phenotypes[int(m_phenotypes.size() * 0.75f)]->getFitness();

		m_phenotypes[0]->setIsElite(m_elites > 0);
		for(unsigned int i = 1; i < m_phenotypes.size(); i++){
			m_phenotypes[i]->setIsElite(m_elites > i);
			if(m_phenotypes[i]->getFitness() != 0){
				valid_breaders++;
			}
			generation.averageFitness += m_phenotypes[i]->getFitness();
		}
		generation.averageFitness /= m_phenotypes.size();
		m_generationHistory.push_back(generation);



		printf("\nGeneration %i\n", m_generations);
		printf("fitness best = %f average = %f\n", generation.bestFitness, generation.averageFitness);
		printf("uniformity = %f\n", generation.averageFitness / generation.bestFitness);
		//generation.bestGenome.print();

		// Copy breader genomes
		vector<sGenome> breaders;
		int num_breaders = int(m_breadingPoolFraction * float(size()));
		if(valid_breaders < num_breaders){
			num_breaders = valid_breaders;
		}
		if(num_breaders < minBreaders)num_breaders = minBreaders;
		for(int i = 0; i < num_breaders; i++){
			breaders.push_back(m_phenotypes[i]->genome);
		}
		
		
		// Breading selection range, selection bias 1.0 - 3.0, default 2.0
		float n2 = float(pow(num_breaders, m_selectionBias));
		

		// Don't change the first ones, they become elites 
		for(unsigned int i = m_elites; i < m_phenotypes.size(); i++){

			// Choose 2 breaders, better performers have better chance, no cloning;

			int i1 = num_breaders - int(pow(getRand(n2), 1.f / m_selectionBias)) - 1;
			int i2 = num_breaders - int(pow(getRand(n2), 1.f / m_selectionBias)) - 1;
			while(i1 == i2){
				i2 = num_breaders - int(pow(getRand(n2), 1.f / m_selectionBias)) - 1;
			}
			breadingDistribution[i1]++;
			breadingDistribution[i2]++;
			m_phenotypes[i]->genome.mate(breaders[i1], breaders[i2], m_mutationRate);
		}

		//printBreadingDistribution();
		
		// Increment generation counter
		m_generations++;
	}



	void setElites(int elites)
	{
		m_elites = elites;
	}
	unsigned int getElites()
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
	
	void printStats()
	{
		int s = m_phenotypes.size();
		int bits = m_phenotypes[0]->genome.bitSize() * s;
		printf("\nPopulation Contains:\n    %i phenotypes\n    %i genes (%3.1f kb)\n",
			s, m_phenotypes[0]->genome.size() * s, float(bits) / 8000.f);
	}


	int minBreaders;

private:

	float getRand(float max)
	{
		return sRandom::getFloat(0,max);
	}

	vector<int> sel_dist;

	int m_generations;
	vector<sGeneration> m_generationHistory;

	static bool sortPhenotypesFunction(sPhenotype *lifeform1, sPhenotype *lifeform2)
	{
		return lifeform1->getFitness() > lifeform2->getFitness();
	}

	float m_mutationRate;
	float m_selectionBias;
	float m_breadingPoolFraction;
	unsigned int m_elites;



	
	vector<sPhenotype*> m_phenotypes;

	vector<int> breadingDistribution;
	void printBreadingDistribution()
	{
		for(int i = 0; i < size(); i++){
			printf("%i : %i\n", i, breadingDistribution[i]);
		}
	}


};



