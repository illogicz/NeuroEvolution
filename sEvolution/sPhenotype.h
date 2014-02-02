#pragma once
#include "../sPhysics/sContainer.h"
#include "sGenome.h"
#include "../sNeuralNet/sNeuralNet.h"


class sPhenotype;

// Virtual class
class sFitnessFunction
{
	public: virtual float operator()(sPhenotype *phenotype) = 0;
};

class sPhenotype : public sContainer, public sStepListener
{
public:

	sPhenotype() : m_isFocus(false){}

	// Derived phenotypes should implement these
	virtual void init(sWorld &world) = 0;
	virtual void build(sWorld &world) = 0;
	virtual void destroy(sWorld &world) = 0;
	virtual void step() = 0;
	virtual b2Vec2 getPosition() = 0;
	virtual b2Vec2 getVelocity() = 0;
	virtual float getFitness()
	{
	    return (*fitnessFunction)(this);		
	};	

	sGenome genome;
	int lifeTime;
	bool alive;

	void born(sWorld &world)
	{
		lifeTime = 0;
		alive = true;
		m_isLeader = false;
		m_aabb_valid = false;
		build(world);
	}

	void die()
	{
		destroy(*m_world);
		alive = false;
	}

	virtual void onBeforeStep()
	{
		if(alive){
			step();
			lifeTime++;
		}
	}
	virtual void onAfterStep()
	{
		m_aabb_valid = false;
		//m_aabb = sContainer::getAABB();
	};

	b2AABB getAABB()
	{
		if(!m_aabb_valid){
			m_aabb = sContainer::getAABB();
			m_aabb_valid = true;
		}
		return m_aabb;
	}


	bool isLeader()
	{
		return m_isLeader;
	}

	bool isElite()
	{
		return m_isElite;
	}

	bool isFocus()
	{
		return m_isFocus;
	}
	int getRank()
	{
		return m_rank;
	}

	// Brain
	sNeuralNet neuralNet;

	sFitnessFunction *fitnessFunction;

protected:
	friend class sSimulationDisplay;
	friend class sPopulation;
	friend class sSimulation;


	void setIsLeader(bool leader)
	{
		m_isLeader = leader;
	}
	void setIsElite(bool elite)
	{
		m_isElite = elite;
	}
	void setIsFocus(bool isFocus)
	{
		m_isFocus = isFocus;
	}
	void setRank(int rank)
	{
		m_rank = rank;
	}

	int m_rank;
	bool m_isLeader;
	bool m_isElite;
	bool m_isFocus;
	b2AABB m_aabb;
	bool m_aabb_valid;
	virtual void addToWorld(sWorld &world)
	{
		born(world);
		sContainer::addToWorld(world);
		world.addStepListener(*this);
	}

	virtual void removeFromWorld(sWorld &world)
	{
		world.removeStepListener(*this);
		sContainer::removeFromWorld(world);
	}

};

