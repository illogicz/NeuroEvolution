#include <Box2D\Box2D.h>
#include <map>
#include "sPhysics\sBody.h"

using std::map;


class State
{

public:

	State(b2World &world):m_world(world){};

	void save()
	{
		b2Body *body = m_world.GetBodyList();

		m_bodyStates.clear();

		while(body != NULL)
		{
			if(body->GetType() == b2BodyType::b2_dynamicBody)
			{
				sBody &sbody = *(sBody*)body->GetUserData();
				m_bodyStates[body] = sbody.getState();
			}
			body = body->GetNext();
		}
	}

	void apply()
	{
		b2Body *body = m_world.GetBodyList();
		m_world.ClearForces();
		//m_world.
		while(body != NULL)
		{
			if(body->GetType() == b2BodyType::b2_dynamicBody)
			{
				if(m_bodyStates.find(body) != m_bodyStates.end())
				{
					sBody &sbody = *(sBody*)body->GetUserData();
					sbody.setState(m_bodyStates[body]);
				}
			}
			body = body->GetNext();
		}
	}

	void interpolate(float t)
	{
		b2Body *body = m_world.GetBodyList();
		while(body != NULL)
		{
			if(body->GetType() == b2BodyType::b2_dynamicBody)
			{
				if(m_bodyStates.find(body) != m_bodyStates.end())
				{
					sBody &sbody = *(sBody*)body->GetUserData();
					sbody.setState(sbody.getState().interpolate(m_bodyStates[body], t));

				}
			}
			body = body->GetNext();
		}
	}

private:


	map<b2Body*, sBodyState> m_bodyStates;





	b2World &m_world;


};