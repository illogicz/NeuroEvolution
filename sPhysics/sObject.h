#pragma once
#include <Box2D\Box2D.h>


enum sObjectType
{
	BODY_OBJECT,
	JOINT_OBJECT,
	CONTAINER_OBJECT
};

class sObject
{

	friend class sContainer;

protected:

	sObject(sObjectType type) : m_type(type)
	{
		m_inWorld = false;
	}
	sObject(sObject &object) : m_type(object.m_type)
	{
		printf("object copy constructor\n");
		m_inWorld = false;
	}

	virtual void copy(sObject &object)
	{
	}

	virtual void addToWorld(b2World &world)
	{
		assert(!m_inWorld);
		m_inWorld = true;
		m_world = &world;
	}
	virtual void removeFromWorld(b2World &world)
	{
		assert(m_inWorld);
		m_inWorld = false;
	}

	const sObjectType m_type;
	bool m_inWorld;
	b2World *m_world;

private:

};