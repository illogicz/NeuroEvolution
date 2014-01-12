#pragma once
#include "sBody.h"


class sUniformBody : public sBody
{

public:

	sUniformBody()
	{
		m_friction = 0.2f;
		m_restitution = 0.3f;
		m_density = 1.0f;
		m_isSensor = false;
	}
	sUniformBody(sUniformBody &body) : sBody(body)
	{
		_copy(body);
	}
	virtual void copy(sUniformBody &body)
	{
		sBody::copy(body);
		_copy(body);
	}

	void setDensity(float32 density)
	{
		m_density = density;
	}
	float32 getDensity()
	{
		return m_density;
	}

	void setFriction(float32 friction)
	{
		m_friction = friction;
	}
	float32 getFriction()
	{
		return m_friction;
	}

	void setRestitution(float32 restitution)
	{
		m_restitution = restitution;
	}
	float32 getRestitution()
	{
		return m_restitution;
	}

	void setFilter(b2Filter filter)
	{
		m_filter = filter;
	}
	b2Filter setFilter()
	{
		return m_filter;
	}

	void setIsSensor(bool isSensor)
	{
		m_isSensor = isSensor;
	}
	bool getIsSensor()
	{
		return m_isSensor;
	}




protected:

	void addToWorld(b2World &world)
	{	
		for(int i=0; i < m_fixtureDefs.size(); i++){
			m_fixtureDefs[i]->density = m_density;
			m_fixtureDefs[i]->friction = m_friction;
			m_fixtureDefs[i]->restitution = m_restitution;
			m_fixtureDefs[i]->filter = m_filter;
			m_fixtureDefs[i]->isSensor = m_isSensor;
		}
		sBody::addToWorld(world);
	}

	float32 m_density;
	float32 m_friction;
	float32 m_restitution;
	b2Filter m_filter;
	bool m_isSensor;

private:

	void _copy(sUniformBody &body)
	{
		m_density = body.m_density;
		m_friction = body.m_friction;
		m_restitution = body.m_restitution;
		m_filter = body.m_filter;
		m_isSensor = body.m_isSensor;
	}

};