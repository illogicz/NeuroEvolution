#pragma once
#include <vector>
#include <Box2D\Box2D.h>
#include "sJoint.h"

using std::vector;

class sMouseJoint : public sJoint
{
	//friend class sWorld; // world must send destruction event
public:

	sMouseJoint()
	{
		m_jointDef = &_jointDef;
		m_maxForce = 10000;
		m_draggingBody = false;
		m_collideConnected = true;
	}

	// Sets mouse position, only relevant when mouse is dragging a body
	void setPosition(b2Vec2 position)
	{
		if(m_draggingBody){
			b2MouseJoint *mj = (b2MouseJoint*)m_joint;
			mj->SetTarget(position);
		}
	}

	// Indicates mouse was pressed at a position
	// return true if a dynamic object was found under the mouse
	bool pressed(b2Vec2 position)
	{
		//
		if(m_inWorld){
			vector<sBody*> bodies = m_world->getBodiesAt(position);
			for(unsigned int i = 0; i < bodies.size(); i++){
				
				// Only drag dynamic bodies
				if(bodies[i]->getType() == DYNAMIC_BODY){
					// Use first body found
					m_bodyB = bodies[i];

					// Set body target
					_jointDef.target = position;
					_jointDef.maxForce = m_maxForce;

					// Add it
					sJoint::_addToWorld(*m_world);

					m_draggingBody = true;
					
					// Set mouse target
					setPosition(position);

					// Ignore other results
					return true;

					
				}
			}
		}
		return false;
	}


	bool released()
	{
		if(m_inWorld && m_draggingBody){
			sJoint::_removeFromWorld(*m_world);
			m_draggingBody = false;
			return true;
		}		
		m_draggingBody = false;
		return false;
	}

	
	void setMaxForce(float32 maxForce)
	{
		_jointDef.maxForce = maxForce;
		m_maxForce = maxForce;
		if(m_inWorld){
			((b2MouseJoint*)m_joint)->SetMaxForce(maxForce);
		}
	}
	

protected:

	// 
	void addToWorld(sWorld &world)
	{
		// Skip call sJoint's method, we will do this when the joint actually gets created
		sObject::addToWorld(world);
	}
	void removeFromWorld(sWorld &world)
	{
		// Skip call sJoint's method, we will do this when the joint actually gets removed
		sObject::removeFromWorld(world);
	}

	bool m_draggingBody;
	float32 m_maxForce;
	b2MouseJointDef _jointDef;



};