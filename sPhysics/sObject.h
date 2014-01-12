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
	}
	sObject(sObject &object) : m_type(object.m_type)
	{
		m_inWorld = false;
	}

	virtual void copy(sObject &object)
	{
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
	
	bool m_inWorld;
	sWorld *m_world;

private:

};