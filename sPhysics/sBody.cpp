#include "sBody.h"


void sBody::addToWorld(sWorld &world)
{
	sObject::addToWorld(world);

	// Set initial state
	m_bodyDef->position = m_state.position;
	m_bodyDef->angle = m_state.angle;
	m_bodyDef->linearVelocity = m_state.linearVelocity;
	m_bodyDef->angularVelocity = m_state.angularVelocity;
	m_bodyDef->linearDamping = m_linearDamping;
	m_bodyDef->angularDamping = m_angularDamping;
	// set type
	m_bodyDef->type = (b2BodyType)m_bodyType;

	// Add body to world
	m_body = world.b2world.CreateBody(m_bodyDef);
	m_body->SetUserData(this);
	//m_body->
	// Add fixtures to body
	int n_fixtures = m_fixtureDefs.size();
	m_fixtures.resize(n_fixtures);
	for(int i = 0; i < n_fixtures; i++){
		m_fixtures[i] = m_body->CreateFixture(m_fixtureDefs[i]);
	}
}

void sBody::removeFromWorld(sWorld &world)
{
	// Saves state
	m_state = getState();
	sObject::removeFromWorld(world);

	// Remove from world
	world.b2world.DestroyBody(m_body);
}