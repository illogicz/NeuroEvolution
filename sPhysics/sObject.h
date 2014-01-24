#pragma once
#include <Box2D\Box2D.h>
//#include "sWorld.h"

class sWorld;

enum sObjectType
{
	BODY_OBJECT,
	JOINT_OBJECT,
	CONTAINER_OBJECT
};

class sObject
{

	friend class sContainer;


public:
	sObject(sObjectType type) : m_type(type)
	{
		m_inWorld = false;
		m_debugDrawEnabled = true;
		m_customColor = b2Color(0,0,0);
		m_alpha = 1.f;
	}
	//virtual ~sObject(){}
	sObject(sObject &object) : m_type(object.m_type)
	{
		m_inWorld = false;
	}

	virtual void copy(sObject &object)
	{
	}

	bool isInWorld()
	{
		return m_inWorld;
	}



	// Debug draw settings
	void setCustomColor(b2Color color)
	{
		m_customColor = color;
	}
	b2Color getCustomColor()
	{
		return m_customColor;
	}

	virtual void setAlpha(float alpha)
	{
		m_alpha = alpha;
	}
	float getAlpha()
	{
		return m_alpha;
	}

	void setDebugDrawEnabled(bool enabled)
	{
		m_debugDrawEnabled = enabled;
	}
	bool getDebugDrawEnabled()
	{
		return m_debugDrawEnabled;
	}



protected:
	virtual void addToWorld(sWorld &world)
	{
		assert(!m_inWorld);
		m_inWorld = true;
		m_world = &world;
	}
	virtual void removeFromWorld(sWorld &world)
	{
		assert(m_inWorld);
		m_inWorld = false;
	}

	// Object type. can't be changed
	const sObjectType m_type;

	bool m_debugDrawEnabled;
	b2Color m_customColor;	
	float m_alpha;

	bool m_inWorld;
	sWorld *m_world;

private:

};