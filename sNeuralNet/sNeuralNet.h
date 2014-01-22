/*//////////////////////////////////////////////////////////////////////////////////////////////////

	NEURAL NETWORK

	The neural net consists of a number of inputs and output, with one or more hidden layers
	in between. At each layer there are synapses connecting every neuron between layers.

	Neurons have a bias value associated with them, and synapses a weight value. Each of these 
	values has a gene associated with it.

	TODO: more explanations


*///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <vector>
#include "..\sEvolution\sGenome.h"

using namespace std;
struct sSynapse;



//==============================================================================================
// Neuron
//==============================================================================================

struct sNeuron
{
	float value;
	float bias;
	sGene *biasGene;
	vector<sSynapse*> inputSynapses;
	vector<sSynapse*> outputSynapses;
	void calcBias()
	{
		bias = biasGene->getValue();
	}
	float activation()
	{
		return tanh_approx(value + bias);
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


//==============================================================================================
// Synapse
//==============================================================================================

struct sSynapse
{
	sGene *weightGene;
	sNeuron *output;
	sNeuron *input;
	float weight;
	void calcWeight()
	{
		weight = weightGene->getValue();
	}
};


//==============================================================================================
// Neural Network
//==============================================================================================

class sNeuralNet
{
public:


	//------------------------------------------------------------------------------------------
	// Neural Net Initialization
	//------------------------------------------------------------------------------------------

	sNeuralNet()
	{
		setInputCount(1);
		setOutputCount(1);
		setHiddenLayerCount(1);
		setHiddenLayerSize(0,4);
		m_maxWeight = 1.f;
		m_maxBias = 1.f;
		m_created = false;
	}

	~sNeuralNet()
	{
		for(unsigned int i = 0; i < m_synapses.size(); i++){
			delete m_synapses[i];
		}
	}

	// Sets the number of input neurons
	void setInputCount(int size)
	{
		inputs.resize(size);
		m_inputCount = size;
	}

	// Sets the number of output neurons
	void setOutputCount(int size)
	{
		outputs.resize(size);
		m_outputCount = size;
	}

	// Sets the number of hidden layers, should be 1 or more
	void setHiddenLayerCount(int size)
	{
		if(size < 1) return;
		m_hiddenLayers.resize(size);
		m_hiddenLayerCount = size;
	}

	// Sets the number of neurons in the scefied hidden layer
	void setHiddenLayerSize(int index, int size)
	{
		if(index >= m_hiddenLayerCount) return;
		m_hiddenLayers[index].resize(size);
	}

	// Sets the Maximum bias for each neuron
	void setMaxBias(float maxBias)
	{
		m_maxBias = maxBias;
	}

	// Set the maximum weight for synapses
	void setMaxWeight(float maxWeight)
	{
		m_maxWeight = maxWeight;
	}

	// Set the value of an input neuron
	// These should be normalized to about -3..3 depending on neuron counts and weight/bias limits
	void setInput(int index, float value)
	{
		if(index >= m_inputCount)return;

		inputs[index].value = value;
	}

	float getInput(int index)
	{
		if(index >= m_inputCount)return 0.f;

		return inputs[index].value;
	}

	// Return activated output value beteem -1 and 1
	float getOutput(int index)
	{
		if(index >= m_outputCount)return 0.f;

		return outputs[index].activation();
	}

	void printStats()
	{
		printf(
"Neural Net contains: \n\
  %i neurons\n\
    %i input\n\
    %i output\n\
    %i hidden\n\
  %i synapses\n",
	
	    m_neurons.size(),
		  m_inputCount, 
		  m_outputCount, 
		  m_neurons.size() - m_inputCount - m_outputCount, 
		m_synapses.size() 
  
       );
	}

	//------------------------------------------------------------------------------------------
	// Create Neural Net
	//------------------------------------------------------------------------------------------
	
	// Generates the network, a genome must be supplied, which is used for weight and bias values
	// TODO: add initial condition other than random, such as all 0, start dumb?
	void create(sGenome &genome)
	{
		m_genome = &genome;
		
		createSynapses(inputs, m_hiddenLayers[0], 0);

		for(unsigned int i = 0; i < m_hiddenLayers.size()-1; i++){
			createSynapses(m_hiddenLayers[i], m_hiddenLayers[i + 1], i + 1);
		}

		createSynapses(m_hiddenLayers[m_hiddenLayers.size()-1], outputs, m_hiddenLayers.size());
		m_created = true;
	}

private: void createSynapses(vector<sNeuron> &_inputs, vector<sNeuron> &_outputs, int layer)
	{

		for(unsigned int i = 0; i < _outputs.size(); i++){
			_outputs[i].biasGene = &m_genome->addGene(getNeuronName(layer + 1,i), -m_maxBias, m_maxBias);
			m_neurons.push_back(&_outputs[i]);
		}


		for(unsigned int i = 0; i < _inputs.size(); i++){
			if(layer == 0){
				_inputs[i].biasGene = &m_genome->addGene(getNeuronName(0,i), -m_maxBias, m_maxBias);
				m_neurons.push_back(&_inputs[i]);
			}

			for(unsigned int j = 0; j < _outputs.size(); j++){

				sSynapse *synapse = new sSynapse; // &m_synapses[m_synapses.size()-1];
				m_synapses.push_back(synapse);

				synapse->weightGene = &m_genome->addGene(getSynapseName(layer,i,j), -m_maxWeight, m_maxWeight);
				synapse->input = &_inputs[i];
				synapse->output = &_outputs[j];

				_inputs[i].outputSynapses.push_back(synapse);
				_outputs[j].inputSynapses.push_back(synapse);

			}
		}
	}


	//------------------------------------------------------------------------------------------
	// Prepare Neural Net from Genes
	//------------------------------------------------------------------------------------------

	// For performance store values from genes when they are changed
public: void prepare()
	{
		for(unsigned int i = 0; i < m_neurons.size(); i++){
			m_neurons[i]->calcBias();
		}
		for(unsigned int i = 0; i < m_synapses.size(); i++){
			m_synapses[i]->calcWeight();
		}
	}


	//------------------------------------------------------------------------------------------
	// Run Neural Net
	//------------------------------------------------------------------------------------------
	

public: void run()
	{
		if(!m_created) return;

		runSynapseLayer(inputs, m_hiddenLayers[0], 0);

		for(unsigned int i = 0; i < m_hiddenLayers.size()-1; i++){
			runSynapseLayer(m_hiddenLayers[i], m_hiddenLayers[i + 1], i + 1);
		}

		runSynapseLayer(m_hiddenLayers[m_hiddenLayers.size()-1], outputs, m_hiddenLayers.size());
	}

private: void runSynapseLayer(vector<sNeuron> &_inputs, vector<sNeuron> &_outputs, int layer)
	{
				
		// reset output values
		unsigned int i = _outputs.size();
		while(i--){
			_outputs[i].value = 0;
		}

		// Loops through inputs
		i = _inputs.size();
		while(i--){

			// Get input activation value
			float activation_value = _inputs[i].activation();

			// Loop through synapses connected to this input
			unsigned int j = _inputs[i].outputSynapses.size();
			while(j--){

				// Add to output neurons value, this is the activation value of the input 
				// neuron multiplied by the synapse weight
				sSynapse *synapse = _inputs[i].outputSynapses[j];
				synapse->output->value += activation_value * synapse->weight;
			}
		}
		
	}






	//------------------------------------------------------------------------------------------
	// Internals
	//------------------------------------------------------------------------------------------

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
	float m_maxWeight;
	float m_maxBias;
	bool m_created;
	vector<int> m_hiddenLayerSize;
	vector<sSynapse*> m_synapses;
	vector<sNeuron*> m_neurons;
	vector<sNeuron> inputs;
	vector<sNeuron> outputs;
	vector<vector<sNeuron>> m_hiddenLayers;

};



