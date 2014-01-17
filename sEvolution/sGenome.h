#pragma once

#include <map>
#include <string>
#include "sGene.h"

using namespace std;

class sGenome
{

public:

	//sGenome mate(Genome &s);


	void addGene(string name, float min, float max, float bits = 16)
	{
		sGene &gene = m_genes[name]; 
		gene.set((max - min) * 0.5, min, max, bits);
		gene.random();
	}
	sGene getGene(string name)
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

	void mate(sGenome &genome1, sGenome &genome2)
	{
		for(map<string, sGene>::iterator i = m_genes.begin(); i != m_genes.end(); ++i){
			i->second.mate(genome1.getGene(i->first), genome2.getGene(i->first));
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

	void print()
	{
		for(map<string, sGene>::iterator i = m_genes.begin(); i != m_genes.end(); ++i){
			printf("%s = 	%f\n", i->first.c_str(), getValue(i->first));
		}
	}

private:

	map<string, sGene> m_genes;



};