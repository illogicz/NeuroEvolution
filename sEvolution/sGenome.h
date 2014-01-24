// TODO: Add different cross over methods

#pragma once
#include <map>
#include <string>
#include "sGene.h"

using namespace std;

class sGenome
{

public:

	//sGenome mate(Genome &s);


	sGene& addGene(string name, double min, double max, int bits = 16, float mutation_rate = 0.01f)
	{
		if(m_genes.find(name) == m_genes.end()){
			sGene &gene = m_genes[name]; 
			gene.set(float((max - min) * 0.5), float(min), float(max), bits, mutation_rate);
			gene.random();
			return gene;
		} else {
			return m_genes[name];
		}
	}
	sGene &getGene(string name)
	{
		return m_genes[name];
	}

	float getValue(const string &name)
	{
		return m_genes[name].getValue();
	}

	void setValue(const string &name, float value)
	{
		m_genes[name].setValue(value);
	}


	void clone(sGenome &genome)
	{
		for(map<string, sGene>::iterator i = m_genes.begin(); i != m_genes.end(); ++i){
			i->second.clone(genome.getGene(i->first));
		}
	}

	void mate(sGenome &genome1, sGenome &genome2, float mutationRate)
	{
		for(map<string, sGene>::iterator i = m_genes.begin(); i != m_genes.end(); ++i){
			i->second.mate(genome1.getGene(i->first), genome2.getGene(i->first), mutationRate);
		}
	}

	void copyGene(string dst, string src, sGeneModifier mod = GENE_RANDOM)
	{
		m_genes[dst] = m_genes[src];
		if(mod == GENE_RANDOM){
			m_genes[dst].random();
		} else if(mod == GENE_INVERSE){
			m_genes[dst].invert();
		}
	}

	int size()
	{
		return m_genes.size();
	}

	void printStats()
	{
		printf("\nGenome Contains:\n    %i genes\n    %i bits of data\n\n", m_genes.size(), bitSize());
		
	}

	int bitSize()
	{
		int bits = 0;
		for(map<string, sGene>::iterator i = m_genes.begin(); i != m_genes.end(); ++i){
		//	printf("%s = 	%f\n", i->first.c_str(), getValue(i->first));
			bits += i->second.getBitCount();
		}
		return bits;
	}

private:

	map<string, sGene> m_genes;



};