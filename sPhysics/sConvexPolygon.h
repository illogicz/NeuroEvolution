#pragma once
#include <vector>
#include "sUniformBody.h"

using std::vector;

class sConvexPolygon : public sUniformBody
{

private:


public:

	sConvexPolygon(float32 x = 0, float32 y = 0, float32 angle = 0)
	{
		m_fixtureDefs.push_back(&s_fixtureDef);
		s_fixtureDef.shape = &s_shape;
		m_bodyDef = &s_bodyDef;
		setPosition(b2Vec2(x, y));
		setAngle(angle);
	}
	~sConvexPolygon(){}
	sConvexPolygon(sConvexPolygon &body) : sUniformBody(body)
	{
		_copy(body);
	}
	virtual void copy(sConvexPolygon &body)
	{
		sUniformBody::copy(body);
		_copy(body);
	}

	void setVerices(vector<b2Vec2> vertices)
	{
		m_vertices = vertices;
	}
	vector<b2Vec2> getVerices()
	{
		return m_vertices;
	}

	void addVertex(b2Vec2 vertex)
	{
		m_vertices.push_back(vertex);
	}
	void addVertex(float32 x, float32 y)
	{
		addVertex(b2Vec2(x, y));
	}
	void clearVerices()
	{
		m_vertices.clear();
	}


protected:

	void addToWorld(sWorld &world)
	{
		//int n = m_vertices.size();
		//b2Vec2 *verts = new b2Vec2[n];
		//for(int i = 0; i < n; i++){
		//	verts[i] = m_vertices[i];
		//}
		//s_shape.Set(verts, n);
		s_shape.Set(&m_vertices[0], m_vertices.size());
		
		sUniformBody::addToWorld(world);
	}

	vector<b2Vec2> m_vertices;

	b2PolygonShape s_shape;
	b2FixtureDef s_fixtureDef;
	b2BodyDef s_bodyDef;

private:

	void _copy(sConvexPolygon &body)
	{
		m_vertices = body.m_vertices;
	}
};