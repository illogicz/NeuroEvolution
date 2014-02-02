#pragma once
#include "sUniformBody.h"


class sRectangle : public sUniformBody
{

private:


public:

	sRectangle(float32 width = 1, float32 height = 1, float32 x = 0, float32 y = 0, float32 angle = 0)
	{
		m_fixtureDefs.push_back(&s_fixtureDef);
		s_fixtureDef.shape = &s_shape;
		m_bodyDef = &s_bodyDef;
		setSize(width, height);
		setPosition(b2Vec2(x, y));
		setAngle(angle);
	}
	sRectangle(sRectangle &rect) : sUniformBody(rect)
	{

		_copy(rect);
	}
	void copy(sRectangle &rect)
	{
		sUniformBody::copy(rect);
		_copy(rect);
	}



	void setSize(b2Vec2 size)
	{
		m_size = size;
	}
	void setSize(float32 width, float32 height)
	{
		setSize(b2Vec2(width, height));
	}
	b2Vec2 getSize()
	{
		return m_size;
	}


protected:

	void addToWorld(sWorld &world)
	{
		s_shape.SetAsBox(m_size.x, m_size.y);		
		sUniformBody::addToWorld(world);
	}

	b2Vec2 m_size;

	static b2PolygonShape s_shape;
	static b2FixtureDef s_fixtureDef;
	static b2BodyDef s_bodyDef;

private:

	void _copy(sRectangle &rect)
	{
		m_size = rect.getSize();
	}
};

b2PolygonShape sRectangle::s_shape;
b2FixtureDef sRectangle::s_fixtureDef;
b2BodyDef sRectangle::s_bodyDef;
