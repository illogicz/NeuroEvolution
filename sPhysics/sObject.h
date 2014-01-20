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
		m_customColor = b2Color(0,0,0);
	}
	//sObject(sObject &object) : m_type(object.m_type)
	//{
	//	m_inWorld = false;
	//}

	virtual void copy(sObject &object)
	{
	}

	bool isInWorld()
	{
		return m_inWorld;
	}

	void setCustomColor(b2Color color)
	{
		m_customColor = color;
	}
	b2Color getCustomColor()
	{
		return m_customColor;
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

	b2Color m_customColor;	
	bool m_inWorld;
	sWorld *m_world;

private:

};