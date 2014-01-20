#pragma once

#include <string>
#include <vector>
#include "../sEvolution/sGenome.h"

using namespace std;
struct sNeuron;

struct sSynapse
{
	sGene *weightGene;
	sNeuron *output;
	sNeuron *input;
	float getWeight()
	{
		return weightGene->getValue();
	}
};

struct sNeuron
{
	float value;
	sGene *biasGene;
	vector<sSynapse*> inputSynapses;
	vector<sSynapse*> outputSynapses;
	float getBias()
	{
		return biasGene->getValue();
	}
	float activation()
	{
		return tanh_approx(value);
	}

	float tanh_approx(float x)
	{
		if( x < -3.f )
			return -1.f;
		else if( x > 3.f )
			return 1.f;
		else
			return x * ( 27.f + x * x ) / ( 27.f + 9.f * x * x );
	}
};

class sNeuralNet
{
public:

	sNeuralNet()
	{
		setInputCount(1);
		setOutputCount(1);
		setHiddenLayerCount(1);
		setHiddenLayerSize(0,4);
	}

	void setInputCount(int size)
	{
		inputs.resize(size);
		m_inputCount = size;
	}

	void setOutputCount(int size)
	{
		outputs.resize(size);
		m_outputCount = size;
	}

	void setHiddenLayerCount(int size)
	{
		m_hiddenLayers.resize(size);
		m_hiddenLayerCount = size;
	}
	void setHiddenLayerSize(int index, int size)
	{
		if(index >= m_hiddenLayerCount) return;
		m_hiddenLayers[index].resize(size);
	}

	void create(sGenome &genome)
	{
		m_genome = &genome;
		
		createSynapses(inputs, m_hiddenLayers[0], 0);

		for(int i = 0; i < m_hiddenLayers.size()-1; i++){
			createSynapses(m_hiddenLayers[i], m_hiddenLayers[i + 1], i + 1);
		}

		createSynapses(m_hiddenLayers[m_hiddenLayers.size()-1], outputs, m_hiddenLayers.size());

	}

	void createSynapses(vector<sNeuron> &_inputs, vector<sNeuron> &_outputs, int layer)
	{

		for(int i = 0; i < _outputs.size(); i++){
			_outputs[i].biasGene = &m_genome->addGene(getNeuronName(layer + 1,i), -0, 0);
		}


		for(int i = 0; i < _inputs.size(); i++){
			if(layer == 0){
				_inputs[i].biasGene = &m_genome->addGene(getNeuronName(0,i), -0, 0);
			}

			for(int j = 0; j < _outputs.size(); j++){

				sSynapse *synapse = new sSynapse; // &m_synapses[m_synapses.size()-1];
				m_synapses.push_back(synapse);

				synapse->weightGene = &m_genome->addGene(getSynapseName(layer,i,j), -1, 1);
				synapse->input = &_inputs[i];
				synapse->output = &_outputs[j];

				_inputs[i].outputSynapses.push_back(synapse);
				_outputs[j].inputSynapses.push_back(synapse);

			}
		}
	}

	void run()
	{
		//printf("=====================================================================\n");

		// input values should be set first

		runSynapseLayer(inputs, m_hiddenLayers[0], 0);

		for(int i = 0; i < m_hiddenLayers.size()-1; i++){
			runSynapseLayer(m_hiddenLayers[i], m_hiddenLayers[i + 1], i + 1);
		}

		runSynapseLayer(m_hiddenLayers[m_hiddenLayers.size()-1], outputs, m_hiddenLayers.size());
	}

	void runSynapseLayer(vector<sNeuron> &_inputs, vector<sNeuron> &_outputs, int layer)
	{

		//printf("Run Synapse layer %i \n", layer);

		for(int i = 0; i < _outputs.size(); i++){

			_outputs[i].value = _outputs[i].getBias();

			//printf("	%i : Inputs: \n", i);

			for(int j = 0; j < _outputs[i].inputSynapses.size(); j++){

				sSynapse *synapse = _outputs[i].inputSynapses[j];

				float w = synapse->getWeight();
				float a = synapse->input->activation();
				float t  = w + a;

				//printf("		a %f * w %f = %f \n", a, w, t);
				// Actual calculation
				_outputs[i].value += t;

			}
			//printf("		 = %f \n", _outputs[i].value);
		}
		//printf("\n");
	}




	vector<sNeuron> inputs;
	vector<sNeuron> outputs;

private:


	string getSynapseName(int layer, int input, int output)
	{
		return "SY_" + to_string(layer) + 
			 "_" + to_string(input) + 
			 "_" + to_string(output);
	}	
	
	string getNeuronName(int layer, int index)
	{
		return "NR_" + to_string(layer) + 
			 "_" + to_string(index);
	}

	sGenome *m_genome;
	int m_inputCount;
	int m_outputCount;
	int m_hiddenLayerCount;
	vector<int> m_hiddenLayerSize;
	vector<sSynapse*> m_synapses;

	vector<vector<sNeuron>> m_hiddenLayers;

};



