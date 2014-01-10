#pragma once
#include <vector>
#include "sObject.h"
#include "sJoint.h"

using std::vector;



struct BodyState
{
	float32 angle;
	float32 angularVelocity;
	b2Vec2 position;
	b2Vec2 linearVelocity;
};

enum sBodyType
{
	STATIC_BODY = b2_staticBody,
	DYNAMIC_BODY = b2_dynamicBody,
	KINEMATIC_BODY = b2_kinematicBody
};

class sBody : public sObject
{
	friend class sJoint;

public:

	sBody() : sObject(BODY_OBJECT)
	{
		m_density = 1;
		m_bodyType = DYNAMIC_BODY;
	}



	~sBody()
	{
		if(m_inWorld){
			removeFromWorld(*m_world);
		}
	}





	void setType(sBodyType type)
	{
		m_bodyType = type;
	}
	sBodyType setType()
	{
		return m_bodyType;
	}

	BodyState getState()
	{
		if(m_inWorld){
			BodyState state;
			state.position = m_body->GetPosition();
			state.angle = m_body->GetAngle();
			state.angularVelocity = m_body->GetAngularVelocity();
			state.linearVelocity = m_body->GetLinearVelocity();
			return state;
		}
		return m_state;
	}
	void setState(BodyState state)
	{
		if(m_inWorld){
			m_body->SetTransform(state.position, state.angle);
			m_body->SetLinearVelocity(state.linearVelocity);
			m_body->SetAngularVelocity(state.angularVelocity);
		} else {
			m_state = state;
		}
	}


	void setPosition(b2Vec2 position)
	{
		if(!m_inWorld){
			m_state.position = position;
		} else {
			m_body->SetTransform(position, m_body->GetAngle());
		}
	}
	b2Vec2 getPosition()
	{
		return m_inWorld ? m_body->GetPosition() : m_state.position;
	}


	void setAngle(float32 angle)
	{
		if(!m_inWorld){
			m_state.angle = angle;
		} else {
			m_body->SetTransform(m_body->GetPosition(), angle);
		}
	}
	float32 getAngle()
	{
		return m_inWorld ? m_body->GetAngle() : m_state.angle;
	}


	void setLinearVelocity(b2Vec2 velocity)
	{
		if(!m_inWorld){
			m_state.linearVelocity = velocity;
		} else {
			m_body->SetLinearVelocity(velocity);
		}
	}
	b2Vec2 getLinearVelocity()
	{
		return m_inWorld ? m_body->GetLinearVelocity() : m_state.linearVelocity;
	}


	void setAngularVelocity(float32 velocity)
	{
		if(!m_inWorld){
			m_state.angularVelocity = velocity;
		} else {
			m_body->SetAngularVelocity(velocity);
		}
	}
	float32 getAngularVelocity()
	{
		return m_inWorld ? m_body->GetAngle() : m_state.angle;
	}



	b2Body *m_body;  // temp public

protected:
	

	virtual void addToWorld(b2World &world)
	{
		sObject::addToWorld(world);

		// Set initial state
		m_bodyDef->position = m_state.position;
		m_bodyDef->angle = m_state.angle;
		m_bodyDef->linearVelocity = m_state.linearVelocity;
		m_bodyDef->angularVelocity = m_state.angularVelocity;
		// set type
		m_bodyDef->type = (b2BodyType)m_bodyType;

		// Add body to world
		m_body = world.CreateBody(m_bodyDef);
		m_body->SetUserData(this);
		
		// Add fixtures to body
		int n_fixtures = m_fixtureDefs.size();
		m_fixtures.resize(n_fixtures);
		for(int i = 0; i < n_fixtures; i++){
			m_fixtures[i] = m_body->CreateFixture(m_fixtureDefs[i]);
		}
	}

	virtual void removeFromWorld(b2World &world)
	{
		// Saves state
		m_state = getState();
		sObject::removeFromWorld(world);

		// Remove from world
		world.DestroyBody(m_body);
	}

	sBodyType m_bodyType;
	float32 m_density;


	BodyState m_state;
	
	b2BodyDef *m_bodyDef;                // Body definition pointer, must be set by deriving class
	vector<b2FixtureDef*> m_fixtureDefs;
	vector<b2Fixture*> m_fixtures;

	
};