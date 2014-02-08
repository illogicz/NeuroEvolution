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
		m_makeLoop = false;
	}

	void setVertexes(std::vector<b2Vec2> vertices)
	{
		m_vertices = vertices;
	}
	void addVertex(float32 x, float32 y)
	{
		addVertex(b2Vec2(x,y));
	}
	void addVertex(b2Vec2 vertex)
	{
		m_vertices.push_back(vertex);
	}
	void clear()
	{
		m_vertices.clear();
	}
	void makeLoop(bool loop)
	{
		m_makeLoop = loop;
	}

protected:

	
	void addToWorld(sWorld &world)
	{
		printf("chain add to world \n");
		_shape.m_count = 0;
		_shape.m_vertices = NULL;
		if(m_makeLoop){
			_shape.CreateLoop(&m_vertices[0], m_vertices.size());
		} else {
			_shape.CreateChain(&m_vertices[0], m_vertices.size());
		}
		sUniformBody::addToWorld(world);
	}
	void removeFromWorld(sWorld &world)
	{
		printf("chain remove from world \n");
		sUniformBody::removeFromWorld(world);
	}
	std::vector<b2Vec2> m_vertices;

	bool m_makeLoop;
	b2ChainShape _shape;
	b2FixtureDef _fixtureDef;
	b2BodyDef _bodyDef;
};