#pragma once
#include <random>


enum sGeneModifier
{
	GENE_MUTATE,
	GENE_RANDOM,
	GENE_INVERSE
};

class sGene
{
	
public:
	sGene(){};

	sGene* set(float value, float min, float max, float bits = 16, float mutation_rate = 0.01f)
	{
		if(bits < 1 || bits > 32){}
		m_min = min;
		m_max = max;
		m_bits = bits;
		m_mutation_rate = mutation_rate;
		setValue(value);
		return this;
	}

	sGene* clone(sGene &gene)
	{
		m_min = gene.m_min;
		m_max = gene.m_max;
		m_bits = gene.m_bits;
		m_data = gene.m_data;
		return this;
	}


	float getValue()
	{
		unsigned int bin = grayToBinary(m_data);
		return m_min + float(bin) / ((1 << m_bits)) * (m_max - m_min);
	}
	
	void setValue(float value)
	{
		unsigned int bin = unsigned int((value - m_min) / (m_max - m_min) * ((1 << m_bits)));
		m_data = binaryToGray(bin);
	}

	unsigned int getBinaryValue()
	{
		return grayToBinary(m_data);
	}
	void setBinaryValue(unsigned int bin)
	{
		m_data = binaryToGray(bin);
	}
	float getNormalizedValue()
	{
		return float(grayToBinary(m_data)) / ((1 << m_bits));
	}

	void random()
	{
		m_data = unsigned int((float(rand()) / RAND_MAX) * ((1 << m_bits)));
	}

	void setMutationRate(float mutationRate)
	{
		m_mutation_rate = mutationRate;
	}

	void invert()
	{
		float v = getValue();
		float t = m_min;
		m_min = -m_max;
		m_max = -t;
		setValue(-v);
	}

	void mate(const sGene &parent1, const sGene &parent2, float mutationRate = 0.01f)
	{
		m_data = 0;
		int mask = 0x01;
		for(int i = 0; i < m_bits; i++){
			bool b = (float(rand()) / RAND_MAX) > 0.5f;
			float r = (float(rand()) / RAND_MAX);
			if(r < mutationRate){
				if(b){
					m_data |= mask;
				} else {
					m_data &= ~mask;
				}
			} else {
				m_data |= (b ? parent1.m_data : parent2.m_data) & mask;
			}
			mask <<= 1;
		}

	}


private:

	float m_mutation_rate;
	int m_bits;
	float m_max;
	float m_min;
	unsigned int m_data;


	// Convert data to/from gray coded binary
	// Source: http://en.wikipedia.org/wiki/Gray_code#Converting_to_and_from_Gray_code

	unsigned int binaryToGray(unsigned int num)
	{
			return (num >> 1) ^ num;
	}
 
	unsigned int grayToBinary(unsigned int num)
	{
		unsigned int mask;
		for (mask = num >> 1; mask != 0; mask = mask >> 1)
		{
			num = num ^ mask;
		}
		return num;
	}



};