#pragma once
#include "sObject.h"
//#include "sBody.h"
//#include "sJoint.h"
//#include "sWorld.h"
#include <set>

using std::set;

class sWorld;

class sContainer : public sObject
{

public:

	sContainer() : sObject(CONTAINER_OBJECT){}


	void add(sObject *object)
	{
		if(!contains(object)){
			if(m_inWorld)object->addToWorld(*m_world);
			m_children.insert(object);
		}
	}
	void remove(sObject *object)
	{
		if(contains(object)){
			if(m_inWorld)object->removeFromWorld(*m_world);
			m_children.erase(object);
		}
	}
	bool contains(sObject *object)
	{
		return m_children.find(object) != m_children.end();
	}


protected:


	virtual void addToWorld(sWorld &world)
	{
		sObject::addToWorld(world);
		// Add bodies and containers first
		for(set<sObject*>::iterator i = m_children.begin(); i != m_children.end(); ++i){
			if((*i)->m_type != JOINT_OBJECT){
				(*i)->addToWorld(world);
			}
		}

		// Add joints after
		for(set<sObject*>::iterator i = m_children.begin(); i != m_children.end(); ++i){
			if((*i)->m_type == JOINT_OBJECT){
				(*i)->addToWorld(world);
			}
		}
	}

	virtual void removeFromWorld(sWorld &world)
	{
		sObject::removeFromWorld(world);
		for(set<sObject*>::iterator i = m_children.begin(); i != m_children.end(); ++i){
			(*i)->removeFromWorld(world);
		}		
	}

	set<sObject*> m_children;
	//set<sBody*> m_bodies;

};