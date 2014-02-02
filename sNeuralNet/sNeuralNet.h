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
struct sSynapse;



//==============================================================================================
// Neuron
//==============================================================================================

struct sNeuron
{
	float accumulator;
	float bias;
	float feedback;
	bool useFeedback;
	sGene *biasGene;
	sGene *feedbackGene;
	vector<sSynapse*> inputSynapses;
	vector<sSynapse*> outputSynapses;
	int layer;
	float lastActivation;
	void prepare()
	{
		accumulator = 0;
		lastActivation = 0;
		bias = biasGene->getValue();
		if(useFeedback){
			feedback = feedbackGene->getValue();
		}
	}
	float activation()
	{
		lastActivation = tanh_approx(accumulator + bias);
		return lastActivation;
	}
	int order; // for rendering purposes only
};


//==============================================================================================
// Synapse
//==============================================================================================

struct sSynapse
{
	sGene *weightGene;
	sGene *enabledGene;
	sNeuron *output;
	sNeuron *input;
	int layer;
	float weight;
	bool enabled;
	float enableThreshold;

	float getValue()
	{
		if(enabled){
			return input->lastActivation * weight;
		}
		return 0;
	}

	void prepare()
	{
		enabled = enabledGene->getValue() < enableThreshold;
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
		m_maxWeight = 3.f;
		m_maxBias = 1.f;
		m_maxFeedback = 1.f;
		m_created = false;
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
		inputs.resize(size);
		m_inputCount = size;
	}
	int getInputCount()
	{
		return m_inputCount;
	}

	// Sets the number of output neurons
	void setOutputCount(int size)
	{
		outputs.resize(size);
		m_outputCount = size;
	}
	int getOutputCount()
	{
		return m_outputCount;
	}

	// Sets the number of hidden layers, should be 1 or more
	void setHiddenLayerCount(int size)
	{
		if(size < 1) return;
		m_hiddenLayers.resize(size);
		m_hiddenLayerCount = size;
		m_weightDistributions.resize(size + 1, 2.f);
		m_biasDistributions.resize(size + 2, 2.f);
		m_feedbackDistributions.resize(size, 2.f);
		m_useFeedback.resize(size, false);
		m_synapseThresholds.resize(size + 1, 0.1f);
	}

	int getHiddenLayerCount()
	{
		return m_hiddenLayerCount;
	}

	// Sets the number of neurons in the scefied hidden layer
	void setHiddenLayerSize(int index, int size)
	{
		if(index >= m_hiddenLayerCount) return;
		m_hiddenLayers[index].resize(size);
	}
	int getHiddenLayerSize(int index)
	{
		if(index >= m_hiddenLayerCount) return 0;
		return m_hiddenLayers[index].size();
	}

	// Sets the Maximum bias for each neuron
	void setMaxBias(float maxBias)
	{
		m_maxBias = maxBias;
	}
	float getMaxBias()
	{
		return m_maxBias;
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

	// Neural feedback
	void setMaxFeedback(float maxFeedback)
	{
		m_maxFeedback = maxFeedback;
	}
	float getMaxFeedback()
	{
		return m_maxFeedback;
	}
	void setUseFeedback(int hiddenLayer, bool useFeedback)
	{
		m_useFeedback[hiddenLayer] = useFeedback;
	}
	bool getUseFeedback(int hiddenLayer)
	{
		return m_useFeedback[hiddenLayer];
	}

	// Set the value of an input neuron
	// These should be normalized to about -3..3 depending on neuron counts and weight/bias limits
	void setInput(int index, float value)
	{
		if(index >= m_inputCount)return;

		inputs[index].accumulator = value;
	}
	void interpolateInput(int index, float value, float t)
	{
		if(index >= m_inputCount)return;

		inputs[index].accumulator += (value - inputs[index].accumulator) * t;
	}
	float getInput(int index)
	{
		if(index >= m_inputCount)return 0.f;

		return inputs[index].accumulator;
	}

	// Return activated output value beteem -1 and 1
	float getOutput(int index)
	{
		if(index >= m_outputCount)return 0.f;
		return outputs[index].activation();
	}
	void setName(string name)
	{
		m_name = name;
	}

	// Weihgt distributions for a synapse layers
	void setWeightDistribution(int layerIndex, float distribution)
	{
		m_weightDistributions[layerIndex] = distribution;
	}
	float getWeightDistribution(int layerIndex)
	{
		return m_weightDistributions[layerIndex];
	}

	// Bias Distributions for neuron layers
	void setBiasDistribution(int layerIndex, float distribution)
	{
		m_biasDistributions[layerIndex] = distribution;
	}
	float getBiasDistribution(int layerIndex)
	{
		return m_biasDistributions[layerIndex];
	}

	// Feedback distribution
	void setFeedbackDistribution(int layerIndex, float distribution)
	{
		m_feedbackDistributions[layerIndex] = distribution;
	}
	float getFeedbackDistribution(int layerIndex)
	{
		return m_feedbackDistributions[layerIndex];
	}

	// Synapse thresholds
	void setSynapseThreshold(int layerIndex, float threshold)
	{
		m_synapseThresholds[layerIndex] = threshold;
	}
	float getSynapseThreshold(int layerIndex)
	{
		return m_synapseThresholds[layerIndex];
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

	vector<sNeuron> &getInputNeurons()
	{
		return inputs;
	}
	vector<sNeuron> &getOutputNeurons()
	{
		return outputs;
	}
	vector<sNeuron> &getHiddenNeurons(int layer)
	{
		return m_hiddenLayers[layer];
	}

	//------------------------------------------------------------------------------------------
	// Create Neural Net
	//------------------------------------------------------------------------------------------
	
	// Generates the network, a genome must be supplied, which is used for weight and bias values
	// TODO: add initial condition other than random, such as all 0, start dumb?
	void create(sGenome &genome)
	{
		m_genome = &genome;
		
		if(m_hiddenLayerCount == 0){

			createLayer(inputs, outputs, 0);

		} else {
			createLayer(inputs, m_hiddenLayers[0], 0);

			for(unsigned int i = 0; i < m_hiddenLayers.size()-1; i++){
				createLayer(m_hiddenLayers[i], m_hiddenLayers[i + 1], i + 1);
			}

			createLayer(m_hiddenLayers[m_hiddenLayers.size()-1], outputs, m_hiddenLayers.size());
		}
		randomize();
		
		m_created = true;
	}

	// Initial random conditions. This needs more care and investigation
	void randomize()
	{  
		//int maxruns = 100; // temp
		//while(maxruns--){
			for(unsigned int i = 0; i < m_neurons.size(); i++){

				int layer = m_neurons[i]->layer;
				float dist = m_biasDistributions[layer];
				float bias = getRandomDistribution(dist) * m_maxBias;
				m_neurons[i]->biasGene->setValue(bias);
				if(m_neurons[i]->useFeedback){
					dist = m_feedbackDistributions[layer - 1];
					float feedback = getRandomDistribution(dist, false) * m_maxFeedback;
					m_neurons[i]->feedbackGene->setValue(feedback);
				}
			}

			for(unsigned int i = 0; i < m_synapses.size(); i++){

				int layer = m_synapses[i]->layer;
				float dist = m_weightDistributions[layer];
				float weight = getRandomDistribution(dist) * m_maxWeight;
				m_synapses[i]->weightGene->setValue(weight);

			}

			//prepare();
			//run();
			
			//if(abs(getOutput(0)) < 0.5f){
			//	break;
			//	printf("%f \n", getOutput(0));
			//}

		//}
	}

	// Builds a synapse layer. Can be overridden for custom synapse connections
protected: virtual void createLayer(vector<sNeuron> &_inputs, vector<sNeuron> &_outputs, int layer)
	{

		for(unsigned int i = 0; i < _outputs.size(); i++){
			_outputs[i].order = i;
			_outputs[i].layer = layer + 1;
			_outputs[i].biasGene = &m_genome->addGene(getNeuronName(layer + 1,i), -m_maxBias, m_maxBias);
			if(layer < m_hiddenLayerCount && m_useFeedback[layer]){
				string name = getNeuronName(layer + 1,i) + "_FB";
				_outputs[i].useFeedback = true;
				_outputs[i].feedbackGene = &m_genome->addGene(name, 0, m_maxFeedback);
			} else {
				_outputs[i].useFeedback = false;
				_outputs[i].feedback = 0;
			}
			m_neurons.push_back(&_outputs[i]);
		}


		for(unsigned int i = 0; i < _inputs.size(); i++){
			if(layer == 0){
				_inputs[i].order = i;
				_inputs[i].layer = layer;
				_inputs[i].feedback = 0;
				_inputs[i].useFeedback = false;
				_inputs[i].biasGene = &m_genome->addGene(getNeuronName(0,i), -m_maxBias, m_maxBias);
				m_neurons.push_back(&_inputs[i]);
			}

			for(unsigned int j = 0; j < _outputs.size(); j++){

				sSynapse *synapse = new sSynapse;
				m_synapses.push_back(synapse);

				synapse->weightGene = &m_genome->addGene(getSynapseName(layer,i,j) + "_W", -m_maxWeight, m_maxWeight);
				synapse->enabledGene = &m_genome->addGene(getSynapseName(layer,i,j) + "_E", 0,1);
				synapse->enableThreshold = 3.f / _outputs.size();//m_synapseThresholds[layer];
				synapse->input = &_inputs[i];
				synapse->output = &_outputs[j];
				synapse->layer = layer;

				_inputs[i].outputSynapses.push_back(synapse);
				_outputs[j].inputSynapses.push_back(synapse);

			}
		}
	}

	float getRandomDistribution(float distribution, bool neg = true)
	{
		float v = sRandom::getFloat(0,1);
		v = pow(v, distribution);
		if(neg && sRandom::getBool()){
			v *= -1;
		}
		return v;
	}

	//------------------------------------------------------------------------------------------
	// Prepare Neural Net from Genes
	//------------------------------------------------------------------------------------------

	// Get values from genes when they are changed
public: void prepare()
	{
		for(unsigned int i = 0; i < m_neurons.size(); i++){
			m_neurons[i]->prepare();
		}
		for(unsigned int i = 0; i < m_synapses.size(); i++){
			m_synapses[i]->prepare();
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
			_outputs[i].accumulator = _outputs[i].lastActivation * _outputs[i].feedback;
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
				if(synapse->enabled){
					synapse->output->accumulator += activation_value * synapse->weight;
				}
			}
		}
		
	}




private:
	
	// Sort neurons according to position and strength of connected neurons
	// For display only
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



	//------------------------------------------------------------------------------------------
	// Internals
	//------------------------------------------------------------------------------------------

	string getSynapseName(int layer, int input, int output)
	{
		return m_name + "_SY_" + to_string(layer) + 
			 "_" + to_string(input) + 
			 "_" + to_string(output);
	}	
	
	string getNeuronName(int layer, int index)
	{
		return m_name + "_NR_" + to_string(layer) + 
			 "_" + to_string(index);
	}

	sGenome *m_genome;
	int m_inputCount;
	int m_outputCount;
	int m_hiddenLayerCount;
	float m_maxWeight;
	float m_maxBias;
	float m_maxFeedback;
	bool m_created;
	string m_name;
	vector<bool> m_useFeedback;
	vector<int> m_hiddenLayerSize;
	vector<sSynapse*> m_synapses;
	vector<sNeuron*> m_neurons;
	vector<sNeuron> inputs;
	vector<sNeuron> outputs;
	vector<vector<sNeuron>> m_hiddenLayers;
	vector<float> m_weightDistributions;
	vector<float> m_biasDistributions;
	vector<float> m_feedbackDistributions;
	vector<float> m_synapseThresholds;

};



