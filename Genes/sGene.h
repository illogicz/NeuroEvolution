#pragma once
#include <random>

class sGene
{
	
public:
	sGene()
	{
		mutation_rate = 0.01f;
	}

	sGene(float value, float min, float max, float bits = 16)
	{
		if(bits < 1 || bits > 32){}
		m_min = min;
		m_max = max;
		m_bits = bits;
		setValue(value);
		mutation_rate = 0.01f;
	}


	sGene* set(float value, float min, float max, float bits = 16)
	{
		if(bits < 1 || bits > 32){}
		m_min = min;
		m_max = max;
		m_bits = bits;
		setValue(value);
		return this;
	}

	sGene* copy(sGene &gene)
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

	void random()
	{
		m_data = unsigned int((float(rand()) / RAND_MAX) * ((1 << m_bits)));
	}

	void mate(const sGene *parent1, const sGene *parent2)
	{
		m_data = 0;
		int mask = 0x01;
		for(int i = 0; i < m_bits; i++){
			bool b = (float(rand()) / RAND_MAX) > 0.5f;
			float r = (float(rand()) / RAND_MAX);
			if(r < mutation_rate){
				if(b){
					m_data |= mask;
				} else {
					m_data &= ~mask;
				}
			} else {
				m_data |= (b ? parent1->m_data : parent2->m_data) & mask;
			}
			mask <<= 1;
		}

	}

	float mutation_rate;
	int m_bits;
	float m_max;
	float m_min;
	unsigned int m_data;

private:


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