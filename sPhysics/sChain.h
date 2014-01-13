#pragma once
#include "sUniformBody.h"


class sChain : public sUniformBody
{

public:

	sChain()
	{
		m_bodyType = STATIC_BODY;
		_fixtureDef.shape = &_shape;
		m_fixtureDefs.push_back(&_fixtureDef);
		m_bodyDef = &_bodyDef;
	}

	void setVertexes(std::vector<b2Vec2> vertexes)
	{
		m_vertexes = vertexes;
	}
	void addVertex(float32 x, float32 y)
	{
		addVertex(b2Vec2(x,y));
	}
	void addVertex(b2Vec2 vertex)
	{
		m_vertexes.push_back(vertex);
	}
	void clear()
	{
		m_vertexes.clear();
	}

protected:

	
	void addToWorld(sWorld &world)
	{
		_shape.CreateChain(&m_vertexes[0], m_vertexes.size());
		sUniformBody::addToWorld(world);
	}

	std::vector<b2Vec2> m_vertexes;

	b2ChainShape _shape;
	b2FixtureDef _fixtureDef;
	b2BodyDef _bodyDef;
};