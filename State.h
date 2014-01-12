#include <Box2D\Box2D.h>
#include <map>

using std::map;


class State
{

public:

	State(b2World &world):m_world(world){};

	void save()
	{
		b2Body *body = m_world.GetBodyList();

		while(body != NULL)
		{
			if(body->GetType() == b2BodyType::b2_dynamicBody)
			{
				BodyState &bodyState = m_bodyStates[body];
				bodyState.angle = body->GetAngle();
				bodyState.angularVelocity = body->GetAngularVelocity();
				bodyState.position = body->GetPosition();
				bodyState.linearVelocity = body->GetLinearVelocity();
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
					BodyState &bodyState = m_bodyStates[body];	
					body->SetAngularVelocity(bodyState.angularVelocity);
					body->SetLinearVelocity(bodyState.linearVelocity);
					body->SetTransform(bodyState.position, bodyState.angle);
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
					BodyState &bodyState = m_bodyStates[body];	

					float32 dav = bodyState.angularVelocity - body->GetAngularVelocity();
					body->SetAngularVelocity(body->GetAngularVelocity() + dav * t);

					b2Vec2 dv = bodyState.linearVelocity - body->GetLinearVelocity();
					body->SetLinearVelocity(body->GetLinearVelocity() + t * dv);

					b2Vec2 dp = bodyState.position - body->GetPosition();
					float32 da = bodyState.angle - body->GetAngle();
					body->SetTransform(body->GetPosition() + t * dp, body->GetAngle() + t * da);
				}
			}
			body = body->GetNext();
		}
	}

private:


	struct BodyState
	{
		float32 angle;
		float32 angularVelocity;
		b2Vec2 position;
		b2Vec2 linearVelocity;
	};


	map<b2Body*, BodyState> m_bodyStates;





	b2World &m_world;


};