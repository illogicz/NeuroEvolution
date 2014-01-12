#pragma once
#include "sUniformBody.h"


class sCircle : public sUniformBody
{

private:


public:

	sCircle(float32 radius = 1, float32 x = 0, float32 y = 0)
	{
		printf("circle default constructor");
		m_fixtureDefs.push_back(&s_fixtureDef);
		s_fixtureDef.shape = &s_shape;
		m_bodyDef = &s_bodyDef;
		setRadius(radius);
		setPosition(b2Vec2(x, y));
	}
	sCircle(sCircle &circle) : sUniformBody(circle)
	{

		_copy(circle);
	}

	void copy(sCircle &circle)
	{
		
		sUniformBody::copy(circle);
		_copy(circle);
	}

	void addToWorld(b2World &world)
	{
		s_shape.m_radius = m_radius;
		sUniformBody::addToWorld(world);
	}

	void setRadius(float32 radius)
	{
		m_radius = radius;
	}

	float32 getRadius()
	{
		return m_radius;
	}


protected:



	float32 m_radius;

	static b2CircleShape s_shape;
	static b2FixtureDef s_fixtureDef;
	static b2BodyDef s_bodyDef;

private:

	void _copy(sCircle &circle)
	{
		printf("circle copy");
		setRadius(circle.getRadius());
	}

};

b2CircleShape sCircle::s_shape;
b2FixtureDef sCircle::s_fixtureDef;
b2BodyDef sCircle::s_bodyDef;

