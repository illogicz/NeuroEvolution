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
	sNeuron *output;
	sNeuron *input;
	float weight;
	float enableThreshold;
	void update(float maxWeight, float exponent, float thres)
	{

		float v = weightGene->getValue();

		if(abs(v) < thres){
			weight = 0;
		} else {

			v = (v - (v < 0 ? -thres : thres)) / (1.f - thres);

			if(input == output){ // disable inhibitory feedback, causes jitter
				v = abs(v); 
			}

			weight = pow(abs(v), exponent) * (v < 0 ? -1: 1) * maxWeight;

		}
	}

};


//==============================================================================================
// Neuron
//==============================================================================================

struct sNeuron
{
	bool biasNeuron;
	bool inputNeuron;
	float inputValue;
	vector<sSynapse*> inputSynapses;
	vector<sSynapse*> outputSynapses;
	float accumulator;
	float activation;
	int id;

	void reset()
	{
		accumulator = 0;
		activation = biasNeuron ? 1.f : 0.f;
		inputValue = 0;
	}

	void activate()
	{
		if(biasNeuron) return;
		if(inputNeuron){
			activation = inputValue;
			return;
		}
		accumulator = inputNeuron ? inputValue : 0;
		for(size_t i = 0; i < inputSynapses.size(); i++){
			accumulator += inputSynapses[i]->input->activation * inputSynapses[i]->weight;
		}
		activation = tanh_approx(accumulator);
		//activation = (*activationFunction)(accumulator);
	}
	float (*activationFunction)(float);
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
		m_initialMaxWeight = m_maxWeight;
		m_weightExponent = 2.f;
		m_connectionsPerNeuron = 5.f;
		m_created = false;
		m_biasNeuron.biasNeuron = true;
		m_biasNeuron.id = 0;
		m_biasNeuron.reset();
		m_name = "NN";
		m_neuronID = 0;
	}

	~sNeuralNet()
	{
		for(unsigned int i = 0; i < m_synapses.size(); i++){
			delete m_synapses[i];
		}
	}

	void setGenome(sGenome &genome)
	{
		m_genome = &genome;
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

	void setInitialMaxWeight(float maxWeight)
	{
		m_initialMaxWeight = maxWeight;
	}

	// Set the value of an input neuron
	// These should be normalized to about -3..3 depending on neuron counts and weight/bias limits
	void setInput(int index, float value)
	{
		if(index >= m_inputCount)return;

		m_layers[0][index].inputValue = value;
	}
	void interpolateInput(int index, float value, float t)
	{
		if(index >= m_inputCount)return;
		sNeuron &neuron = m_layers[0][index];
		neuron.inputValue += (value - neuron.inputValue) * t;
	}
	float getInput(int index)
	{
		if(index >= m_inputCount)return 0.f;
		return m_layers[0][index].inputValue;
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
	

	// Initial random conditions. This needs more care and investigation
	void randomize()
	{  
		//float maxw = m_initialMaxWeight / m_maxWeight;
		for(unsigned int i = 0; i < m_synapses.size(); i++){


			//float maxw = 1.f / sqrt(m_synapses[i]->output->inputSynapses.size());
			//if(maxw > 0.25)maxw = 0.25;
			m_synapses[i]->weightGene->setValue(sRandom::getNormal(0.f, 0.3f));
			//m_synapses[i]->weightGene->setValue(sRandom::getFloat(-1, 1));
		}
	}

	// Sets the number of neurons in the specified
	void setNeuronLayer(int layer, int size, bool bias = true, bool feedback = false)
	{
		if(layer >= getLayerCount()) return;
		m_layers[layer].resize(size);
		if(layer == 0){
			m_inputCount = size;
		} else if(layer == m_layers.size() - 1){
			m_outputCount = size;
		}


		for(int i = 0; i < size; i++){

			sNeuron &neuron = m_layers[layer][i];
			neuron.layer = layer;
			neuron.order = i;
			neuron.id = ++m_neuronID;
			neuron.biasNeuron = false;
			neuron.inputNeuron = layer == 0;
			if(layer == getLayerCount() - 1){
				neuron.activationFunction = &tanhf;
			} else {
				neuron.activationFunction = &Sigmoid;
			}
			m_neurons.push_back(&neuron);

		}

		if(bias){
			createBiasSynapses(m_layers[layer]);
		}
		if(feedback){
			createFeedbackSynapses(m_layers[layer]);
		}

	}

	void addSynapseLayer(int input_layer, int output_layer)
	{
		createSynapseLayer(m_layers[input_layer], m_layers[output_layer]);
	}

	void addFeedback(int layer)
	{
		createFeedbackSynapses(m_layers[layer]);
	}

	// Builds a synapse layer.
private: 
	

	void createFeedbackSynapses(vector<sNeuron> &neurons)
	{
		for(size_t i = 0; i < neurons.size(); i++){
			addSynapse(&neurons[i], &neurons[i]);
		}
	}

	void createBiasSynapses(vector<sNeuron> &neurons)
	{
		for(size_t i = 0; i < neurons.size(); i++){
			addSynapse(&m_biasNeuron, &neurons[i]);
		}
	}

	void createSynapseLayer(vector<sNeuron> &_inputs, vector<sNeuron> &_outputs)
	{
		for(size_t j = 0; j < _outputs.size(); j++){
			for(size_t i = 0; i < _inputs.size(); i++){
				addSynapse(&_inputs[i], &_outputs[j]);
			}
		}
	}

	void addSynapse(sNeuron *input, sNeuron *output)
	{
		sSynapse *synapse = new sSynapse;
		m_synapses.push_back(synapse);

		synapse->weightGene = &m_genome->addGene(getSynapseName(input, output) + "_W", -1, 1);
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

		if(!m_created){
			for(size_t i = 0; i < m_layers.size() - 1; i++){
				createSynapseLayer(m_layers[i], m_layers[i+1]);
			}
			randomize();
			m_created = true;
		}

		for(size_t i = 0; i < m_neurons.size(); i++){
			m_neurons[i]->reset();
		}
		for(size_t i = 0; i < m_synapses.size(); i++){
			float thres_out = 6.f / m_synapses[i]->output->inputSynapses.size();
			float thres_in = 6.f / m_synapses[i]->input->outputSynapses.size();
			float thres = max(thres_out, thres_in);
			if(thres > 1)thres = 1;
			m_synapses[i]->update( m_maxWeight, m_weightExponent, 1.f - thres );
			
		}

		//for(int i = 0; i < m_hiddenLayers.size(); i++){
		//	sortHiddenLayer(i);
		//}
	}


	//------------------------------------------------------------------------------------------
	// Run Neural Net
	//------------------------------------------------------------------------------------------
	

    void run()
	{
		if(!m_created) return;

		for(size_t i = 0; i < m_layers.size(); i++){
			for(size_t j = 0; j < m_layers[i].size(); j++){
				m_layers[i][j].activate();
			}
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
	int m_neuronID;
	float m_maxWeight;
	float m_initialMaxWeight;
	float m_weightExponent;
	float m_connectionsPerNeuron;

	bool m_created;
	string m_name;
	sNeuron m_biasNeuron;
	vector<sSynapse*> m_synapses;
	vector<sNeuron*> m_neurons;
	vector<vector<sNeuron>> m_layers;
	

};



