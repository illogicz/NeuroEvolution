/*//////////////////////////////////////////////////////////////////////////////////////////////////

	NEURAL NETWORK

	The neural net consists of a number of inputs and output, with one or more hidden layers
	in between. At each layer there are synapses connecting every neuron between layers.

	Neurons have a bias value associated with them, and synapses a weight value. Each of these 
	values has a gene associated with it.

	TODO: allow restructuring while maintaining gene connections


*///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <vector>
#include "..\sUtils\sMath.h"
#include "..\sEvolution\sGenome.h"

using namespace std;



//==============================================================================================
// Synapse
//==============================================================================================

struct sNeuron;
struct sSynapse
{
	sGene *weightGene;
	sGene *enabledGene;
	sNeuron *output;
	sNeuron *input;
	float weight;
	bool enabled;
	float enableThreshold;
	void update(float maxWeight, float exponent)
	{
		enabled = enabledGene->getValue() < enableThreshold;
		float v = weightGene->getValue();
		weight = pow(abs(v), exponent) * (v < 0 ? -1: 1) * maxWeight;
	}

};


//==============================================================================================
// Neuron
//==============================================================================================

struct sNeuron
{
	bool biasNeuron;
	bool inputNeuron;
	vector<sSynapse*> inputSynapses;
	vector<sSynapse*> outputSynapses;
	float accumulator;
	float activation;
	int id;

	void reset()
	{
		accumulator = 0;
		activation = 0;
	}

	void activate()
	{
		if(biasNeuron) activation = 1;
		if(!inputNeuron){
			accumulator = 0;
			for(int i = 0; i < inputSynapses.size(); i++){
				if(inputSynapses[i]->enabled){
					accumulator += inputSynapses[i]->input->activation * inputSynapses[i]->weight;
				}
			}
		}
		activation = tanh_approx(accumulator);
	}

	int layer;
	int order; // for rendering purposes only
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
		m_maxWeight = 3.f;
		m_weightExponent = 2.f;
		m_connectionsPerNeuron = 5.f;
		m_created = false;
		m_biasNeuron.biasNeuron = true;
		m_name = "NN";
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
		setLayerSize(0,size);
	}
	int getInputCount()
	{
		return getLayerSize(0);
	}

	// Sets the number of output neurons
	void setOutputCount(int size)
	{
		setLayerSize(m_layers.size()-1, size);
	}
	int getOutputCount()
	{
		return getLayerSize(m_layers.size()-1);
	}

	// Sets the number of hidden layers, should be 1 or more
	void setLayerCount(int size)
	{
		if(size < 2) return;
		m_layers.resize(size);
	}
	int getLayerCount()
	{
		return m_layers.size();
	}

	// Sets the number of neurons in the scefied hidden layer
	void setLayerSize(int index, int size)
	{
		if(index >= getLayerCount()) return;
		m_layers[index].resize(size);
		if(index == 0){
			m_inputCount = size;
		} else if(index == m_layers.size() - 1){
			m_outputCount = size;
		}
	}
	int getLayerSize(int index)
	{
		if(index >= getLayerCount()) return 0;
		return m_layers[index].size();
	}

	// Set the maximum weight for synapses
	void setMaxWeight(float maxWeight)
	{
		m_maxWeight = maxWeight;
	}
	float getMaxWeight()
	{
		return m_maxWeight;
	}


	// Set the value of an input neuron
	// These should be normalized to about -3..3 depending on neuron counts and weight/bias limits
	void setInput(int index, float value)
	{
		if(index >= m_inputCount)return;

		m_layers[0][index].accumulator = value;
	}
	void interpolateInput(int index, float value, float t)
	{
		if(index >= m_inputCount)return;

		m_layers[0][index].accumulator += (value - m_layers[0][index].accumulator) * t;
	}
	float getInput(int index)
	{
		if(index >= m_inputCount)return 0.f;
		return m_layers[0][index].accumulator;
	}

	// Return activated output value beteem -1 and 1
	float getOutput(int index)
	{
		if(index >= m_outputCount)return 0.f;
		return m_layers[m_layers.size()-1][index].activation;
	}
	void setName(string name)
	{
		m_name = name;
	}

	// Weight exponents
	void setWeightExponent(float exponent)
	{
		m_weightExponent = exponent;
	}
	float getWeightExponent()
	{
		return m_weightExponent;
	}

	void setConnectionsPerNeuron(float connections)
	{
		m_connectionsPerNeuron = connections;
	}
	float getConnectionsPerNeuron()
	{
		return m_connectionsPerNeuron;
	}


	void printStats()
	{
		printf(
"Neural Net contains: \n\
  %i neurons\n\
  %i synapses\n",
	
	    m_neurons.size(),
		m_synapses.size() 
  
       );
	}

	vector<sNeuron> &getInputNeurons()
	{
		return m_layers[0];
	}
	vector<sNeuron> &getOutputNeurons()
	{
		return m_layers[m_layers.size()-1];
	}
	vector<sNeuron> &getNeurons(int layer)
	{
		return m_layers[layer];
	}

	vector<sNeuron*> &getNeurons()
	{
		return m_neurons;
	}
	vector<sSynapse*> &getSynapses()
	{
		return m_synapses;
	}


	//------------------------------------------------------------------------------------------
	// Create Neural Net
	//------------------------------------------------------------------------------------------
	
	// Generates the network, a genome must be supplied, which is used for weight and bias values
	// TODO: add initial condition other than random, such as all 0, start dumb?
	void create(sGenome &genome)
	{
		m_genome = &genome;
		
		int id = 0;
		for(int j = 0; j < m_layers.size(); j++){
			for(int i = 0; i < m_layers[j].size(); i++){

				sNeuron &neuron = m_layers[j][i];
				neuron.layer = j;
				neuron.order = i;
				neuron.id = id++;
				neuron.biasNeuron = false;
				neuron.inputNeuron = j == 0;
				m_neurons.push_back(&neuron);
			}
		}

		for(int i = 0; i < m_layers.size() - 1; i++){
			createSynapseLayer(m_layers[i], m_layers[i+1]);
		}

		for(int i = 0; i < m_layers.size(); i++){
			createFeedbackSynapses(m_layers[i]);
		}

		for(int j = 1; j < m_layers.size(); j++){
			for(int i = 0; i < m_layers[j].size(); i++){
				sNeuron *neuron = &m_layers[j][i];
				int num_synapses = neuron->inputSynapses.size();
				float thresh = m_connectionsPerNeuron / num_synapses;
				for(int k = 0; k < num_synapses; k++){
					neuron->inputSynapses[k]->enableThreshold = thresh;
				}
			}
		}

		m_created = true;
	}

	// Initial random conditions. This needs more care and investigation
	void randomize()
	{  
		for(unsigned int i = 0; i < m_synapses.size(); i++){
			m_synapses[i]->weightGene->random();
			m_synapses[i]->enabledGene->random();
		}
	}

	// Builds a synapse layer.
protected: 
	

	void createFeedbackSynapses(vector<sNeuron> &neurons)
	{
		for(int i = 0; i < neurons.size(); i++){
			addSynapse(&neurons[i], &neurons[i]);
		}
	}

	void createSynapseLayer(vector<sNeuron> &_inputs, vector<sNeuron> &_outputs)
	{
		for(unsigned int j = 0; j < _outputs.size(); j++){
			addSynapse(&m_biasNeuron, &_outputs[j]);
			for(unsigned int i = 0; i < _inputs.size(); i++){
				addSynapse(&_inputs[i], &_outputs[j]);
			}
		}
	}

	void addSynapse(sNeuron *input, sNeuron *output)
	{
		sSynapse *synapse = new sSynapse;
		m_synapses.push_back(synapse);

		synapse->weightGene = &m_genome->addGene(getSynapseName(input, output) + "_W", -1, 1);
		synapse->enabledGene = &m_genome->addGene(getSynapseName(input, output) + "_E", 0, 1);
		//synapse->enableThreshold = 3.f / _inputs.size();//m_synapseThresholds[layer];
		synapse->input = input;
		synapse->output = output;
		input->outputSynapses.push_back(synapse);
		output->inputSynapses.push_back(synapse);
	}


	//------------------------------------------------------------------------------------------
	// Prepare Neural Net from Genes
	//------------------------------------------------------------------------------------------

	// Get values from genes when they are changed
public: void update()
	{
		for(unsigned int i = 0; i < m_neurons.size(); i++){
			m_neurons[i]->reset();
		}
		for(unsigned int i = 0; i < m_synapses.size(); i++){
			m_synapses[i]->update(m_maxWeight, m_weightExponent);
		}
		//for(int i = 0; i < m_hiddenLayers.size(); i++){
		//	sortHiddenLayer(i);
		//}
	}


	//------------------------------------------------------------------------------------------
	// Run Neural Net
	//------------------------------------------------------------------------------------------
	

public: void run()
	{
		if(!m_created) return;

		for(unsigned int i = 0; i < m_layers.size(); i++){
			runNeuronLayer(m_layers[i]);
		}
	}

private: 
	
	
	void runNeuronLayer(vector<sNeuron> &neurons)
	{
		for(int i = 0; i < neurons.size(); i++){
			neurons[i].activate();
		}
	}



private:
	
	// Sort neurons according to position and strength of connected neurons
	// For display only
	/*
	void sortHiddenLayer(int layer)
	{
		int l = m_hiddenLayers[layer].size();
		vector<pair<float,int>> activityAve(l,make_pair(0,0));
		for(int i = 0; i < l; i++){
			vector<sSynapse*> &input_synapses = m_hiddenLayers[layer][i].inputSynapses;
			float wt = 0;
			for(int j=0; j < input_synapses.size(); j++){
				float w = abs(input_synapses[j]->weight);
				wt += w;
				activityAve[i].first += w * float(input_synapses[j]->input->order);
			}
			vector<sSynapse*> &output_synapses = m_hiddenLayers[layer][i].outputSynapses;
			for(int j=0; j < output_synapses.size(); j++){
				float w = abs(output_synapses[j]->weight);
				wt += w;
				activityAve[i].first += w * float(output_synapses[j]->output->order);
			}
			activityAve[i].first /= wt;
			activityAve[i].second = i;
			//printf("ave = %f \n", activityAve[i]);
		}
		sort(activityAve.begin(), activityAve.end());
		for(int i = 0; i < l; i++){
			m_hiddenLayers[layer][activityAve[i].second].order = i;
		}
	}
	*/


	//------------------------------------------------------------------------------------------
	// Internals
	//------------------------------------------------------------------------------------------

	string getSynapseName(sNeuron* input, sNeuron *output)
	{
		return m_name + "_SY_" + "_" + to_string(input->id) + "_" + to_string(output->id);
	}	

	sGenome *m_genome;
	int m_inputCount;
	int m_outputCount;

	float m_maxWeight;
	float m_weightExponent;
	float m_connectionsPerNeuron;

	bool m_created;
	string m_name;
	sNeuron m_biasNeuron;
	vector<sSynapse*> m_synapses;
	vector<sNeuron*> m_neurons;
	vector<vector<sNeuron>> m_layers;
	

};



