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

	void set(vector<b2Vec2> vertices)
	{
		m_vertices = vertices;
	}

	void add(b2Vec2 vertex)
	{
		m_vertices.push_back(vertex);
	}
	void add(float32 x, float32 y)
	{
		add(b2Vec2(x, y));
	}



protected:

	void addToWorld(sWorld &world)
	{
		int n = m_vertices.size();
		b2Vec2 *verts = new b2Vec2[n];
		for(int i = 0; i < n; i++){
			verts[i] = m_vertices[i];
		}
		s_shape.Set(verts, n);

		sUniformBody::addToWorld(world);
	}

	vector<b2Vec2> m_vertices;

	b2PolygonShape s_shape;
	b2FixtureDef s_fixtureDef;
	b2BodyDef s_bodyDef;
};