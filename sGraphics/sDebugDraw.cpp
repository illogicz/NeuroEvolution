#include "sDebugDraw.h"



void sDebugDraw::DrawShape(b2Fixture* fixture, const b2Transform& xf, sf::Color& color)
{
	switch (fixture->GetType())
	{
	case b2Shape::e_circle:
		{
			b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

			b2Vec2 center = b2Mul(xf, circle->m_p);
			float32 radius = circle->m_radius;
			b2Vec2 axis = b2Mul(xf.q, b2Vec2(1.0f, 0.0f));

			DrawSolidCircle(center, radius, axis, color);
		}
		break;

	case b2Shape::e_edge:
		{
			b2EdgeShape* edge = (b2EdgeShape*)fixture->GetShape();
			b2Vec2 v1 = b2Mul(xf, edge->m_vertex1);
			b2Vec2 v2 = b2Mul(xf, edge->m_vertex2);
			DrawSegment(v1, v2, color);
		}
		break;

	case b2Shape::e_chain:
		{
			b2ChainShape* chain = (b2ChainShape*)fixture->GetShape();
			int32 count = chain->m_count;
			const b2Vec2* vertices = chain->m_vertices;

			b2Vec2 v1 = b2Mul(xf, vertices[0]);
			for (int32 i = 1; i < count; ++i)
			{
				b2Vec2 v2 = b2Mul(xf, vertices[i]);
				DrawSegment(v1, v2, color);
				//DrawCircle(v1, 0.05f, color);
				v1 = v2;
			}
		}
		break;

	case b2Shape::e_polygon:
		{
			b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
			int32 vertexCount = poly->m_count;
			b2Assert(vertexCount <= b2_maxPolygonVertices);
			b2Vec2 vertices[b2_maxPolygonVertices];

			for (int32 i = 0; i < vertexCount; ++i)
			{
				vertices[i] = b2Mul(xf, poly->m_vertices[i]);
			}

			DrawSolidPolygon(vertices, vertexCount, color);
		}
		break;
            
    default:
        break;
	}
}

void sDebugDraw::DrawJoint(b2Joint* joint)
{

	sJoint *sj = (sJoint*)joint->GetUserData();
	if(sj->getAlpha() == 0) return;

	b2Body* bodyA = joint->GetBodyA();
	b2Body* bodyB = joint->GetBodyB();
	const b2Transform& xf1 = bodyA->GetTransform();
	const b2Transform& xf2 = bodyB->GetTransform();
	b2Vec2 x1 = xf1.p;
	b2Vec2 x2 = xf2.p;
	b2Vec2 p1 = joint->GetAnchorA();
	b2Vec2 p2 = joint->GetAnchorB();



	sf::Color color(127, 204, 204, sj->getAlpha() * 255);
	b2WheelJoint *wj;
	b2Vec2 la;

	switch (joint->GetType())
	{
	case e_distanceJoint:
		DrawSegment(p1, p2, color);
		break;

	case e_pulleyJoint:
		{
			b2PulleyJoint* pulley = (b2PulleyJoint*)joint;
			b2Vec2 s1 = pulley->GetGroundAnchorA();
			b2Vec2 s2 = pulley->GetGroundAnchorB();
			DrawSegment(s1, p1, color);
			DrawSegment(s2, p2, color);
			DrawSegment(s1, s2, color);
		}
		break;

	case e_mouseJoint:
		// don't draw this
		break;
	case e_wheelJoint:
		wj = (b2WheelJoint*)joint;
		p1 = bodyA->GetWorldPoint(wj->GetLocalAnchorA() + wj->GetLocalAxisA());

		//DrawSegment(x2, p1, color);
		//DrawSegment(x2, x1, color);
		DrawSegment(x2, p1, color);
		//DrawSegment(p1, p2, color);
		//DrawSegment(x2, p2, color);
		break;
	default:
		DrawSegment(x1, p1, color);
		DrawSegment(p1, p2, color);
		DrawSegment(x2, p2, color);
	}
}
void sDebugDraw::DrawGrid(const b2AABB &aabb)
{

	sf::Color subColor(127, 127, 127, 80);
	sf::Color mainColor(150, 150, 150, 120);
	int startx = int(aabb.lowerBound.x);
	int endx = int(aabb.upperBound.x);
	int starty = int(aabb.lowerBound.y);
	int endy = int(aabb.upperBound.y);
	for(int x = startx; x <= endx; x++){
		addLine(b2Vec2(x, aabb.lowerBound.y), b2Vec2(x, aabb.upperBound.y), x % 10 ? subColor : mainColor);
	}
	for(int y = starty; y <= endy;  y++){
		addLine(b2Vec2(aabb.lowerBound.x, y), b2Vec2(aabb.upperBound.x, y), y % 10 ? subColor : mainColor);
	}
}

void sDebugDraw::DrawDebugData(sWorld &world)
{

	uint32 flags = GetFlags();
	b2Body *m_bodyList = world.b2world.GetBodyList();
	b2Joint *m_jointList = world.b2world.GetJointList();


	// Use broadphase clipping to view for rendering debug data
	b2AABB aabb;
	aabb.lowerBound.x = view_center.x - view_size.x;
	aabb.upperBound.x = view_center.x + view_size.x;
	aabb.lowerBound.y = view_center.y - view_size.y;
	aabb.upperBound.y = view_center.y + view_size.y;

	DrawGrid(aabb);

	if (flags & b2Draw::e_shapeBit)
	{


		AABBQueryCallback aabb_callback;
		world.b2world.QueryAABB(&aabb_callback, aabb);


		//for (b2Body* b = m_bodyList; b; b = b->GetNext())
		//{
		for(int i = 0; i < aabb_callback.fixtures.size(); i++){
			b2Fixture* f = aabb_callback.fixtures[i];
			b2Body *b = f->GetBody();
			const b2Transform& xf = b->GetTransform();

			sBody *sb = (sBody*)b->GetUserData();

			if(!sb->getDebugDrawEnabled())continue;
			
			float a = sb->getAlpha() * 255;
			if(a == 0) continue;

			b2Color cc = sb->getCustomColor();
			if(cc.b || cc.r || cc.g){

				DrawShape(f, xf, sf::Color(cc.r * 255, cc.g * 255, cc.b * 255, a));

			} else {

				if (b->IsActive() == false)
				{
					DrawShape(f, xf, sf::Color(127, 127, 76, a));
				}
				else if (b->GetType() == b2_staticBody)
				{
					DrawShape(f, xf, sf::Color(127, 230, 127, a));
				}
				else if (b->GetType() == b2_kinematicBody)
				{
					DrawShape(f, xf, sf::Color(127, 127, 230, a));
				}
				else if (b->IsAwake() == false)
				{
					DrawShape(f, xf, sf::Color(153, 153, 153, a));
				}
				else
				{
					DrawShape(f, xf, sf::Color(230, 153, 153, a));
				}
			}
		}
	}

	if (flags & b2Draw::e_jointBit)
	{
		for (b2Joint* j = m_jointList; j; j = j->GetNext())
		{

			sJoint *sj = (sJoint*)j->GetUserData();

			if(!sj->getDebugDrawEnabled())continue;

			DrawJoint(j);
		}
	}

	if (flags & b2Draw::e_pairBit)
	{
		b2Color color(0.3f, 0.9f, 0.9f);
		//for (b2Contact* c = world.b2world.GetContactManager().m_contactList; c; c = c->GetNext())
		//{
			//b2Fixture* fixtureA = c->GetFixtureA();
			//b2Fixture* fixtureB = c->GetFixtureB();

			//b2Vec2 cA = fixtureA->GetAABB().GetCenter();
			//b2Vec2 cB = fixtureB->GetAABB().GetCenter();

			//m_debugDraw->DrawSegment(cA, cB, color);
		//}
	}

	/*
	if (flags & b2Draw::e_aabbBit)
	{
		b2Color color(0.9f, 0.3f, 0.9f);
		b2BroadPhase* bp = &m_contactManager.m_broadPhase;

		for (b2Body* b = m_bodyList; b; b = b->GetNext())
		{
			if (b->IsActive() == false)
			{
				continue;
			}

			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				for (int32 i = 0; i < f->m_proxyCount; ++i)
				{
					b2FixtureProxy* proxy = f->m_proxies + i;
					b2AABB aabb = bp->GetFatAABB(proxy->proxyId);
					b2Vec2 vs[4];
					vs[0].Set(aabb.lowerBound.x, aabb.lowerBound.y);
					vs[1].Set(aabb.upperBound.x, aabb.lowerBound.y);
					vs[2].Set(aabb.upperBound.x, aabb.upperBound.y);
					vs[3].Set(aabb.lowerBound.x, aabb.upperBound.y);

					m_debugDraw->DrawPolygon(vs, 4, color);
				}
			}
		}
	}
	*/
	if (flags & b2Draw::e_centerOfMassBit)
	{
		for (b2Body* b = m_bodyList; b; b = b->GetNext())
		{
			b2Transform xf = b->GetTransform();
			xf.p = b->GetWorldCenter();
			DrawTransform(xf);
		}
	}
}


void sDebugDraw::allocate(sf::VertexArray &va, int i, int n)
{
	if(i + n > va.getVertexCount()){
		va.resize(i + n);
	}
}

void sDebugDraw::addTriangle(const b2Vec2 &v1, const b2Vec2 &v2, const b2Vec2 &v3, sf::Color &c)
{
	if(triangles_index + 3 > triangles.getVertexCount()){
		triangles.resize(triangles_index + 3);
	}
	triangles[triangles_index].color = c;
	triangles[triangles_index].position.x = v1.x;
	triangles[triangles_index].position.y = v1.y;
	triangles_index++;
	triangles[triangles_index].color = c;
	triangles[triangles_index].position.x = v2.x;
	triangles[triangles_index].position.y = v2.y;
	triangles_index++;
	triangles[triangles_index].color = c;
	triangles[triangles_index].position.x = v3.x;
	triangles[triangles_index].position.y = v3.y;
	triangles_index++;
}
void sDebugDraw::addLine(const b2Vec2 &v1, const b2Vec2 &v2, sf::Color &c)
{
	if(lines_index + 2 > lines.getVertexCount()){
		lines.resize(lines_index + 2);
	}
	lines[lines_index].position.x = v1.x;
	lines[lines_index].position.y = v1.y;
	lines[lines_index].color = c;
	lines_index++;
	lines[lines_index].position.x = v2.x;
	lines[lines_index].position.y = v2.y;
	lines[lines_index].color = c;
	lines_index++;
}

void sDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, sf::Color& color)
{
	allocate(lines, lines_index, (vertexCount-1)*2);
	for(int i=0; i < vertexCount-1; i++){
		addLine(vertices[i], vertices[i+1], color);
	}
}

void sDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, sf::Color& color)
{
	float a = color.a;
	color.a *= 0.4;

	allocate(triangles, triangles_index, (vertexCount-2)*3);
	for(int i=1; i < vertexCount-1; i++){
		addTriangle(vertices[0], vertices[i], vertices[i+1], color);
	}

	allocate(lines, lines_index, vertexCount*2);
	color.a = a;
	for(int i=0; i < vertexCount; i++){
		addLine(vertices[i], vertices[(i+1) % vertexCount], color);
	}

}

void sDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, sf::Color& color)
{
	sf::CircleShape circle(radius, 20);
	circle.setPosition(center.x, center.y);
	circle.setOutlineColor(color);
	//color.a = 0;
	circle.setFillColor(color);
	m_target->draw(circle, states);
}

void sDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, sf::Color& color)
{

	allocate(lines, lines_index, 2);
	lines[lines_index].position.x = center.x;
	lines[lines_index].position.y = center.y;
	lines[lines_index].color = color;
	lines_index++;
	lines[lines_index].position.x = center.x + axis.x * radius;
	lines[lines_index].position.y = center.y + axis.y * radius;
	lines[lines_index].color = color;
	lines_index++;

	sf::CircleShape circle(radius, 20);
	circle.setPosition(center.x-radius, center.y-radius);
	circle.setOutlineThickness(1.f/50);
	circle.setOutlineColor(color);
	color.a *= 0.4;
	circle.setFillColor(sf::Color(color.r*0xFF, color.g*0xFF, color.b*0xFF, 0x60));
	m_target->draw(circle, states);

}

void sDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, sf::Color& color)
{
	//sf::Color c = sf::Color(color.r*0xFF, color.g*0xFF, color.b*0xFF, 0xFF);
	addLine(p1, p2, color);
}

void sDebugDraw::DrawTransform(const b2Transform& xf)
{
}

void sDebugDraw::DrawPoint(const b2Vec2& p, float32 size, sf::Color& color)
{
}

void sDebugDraw::DrawString(int x, int y, const char* string, ...)
{
}

void sDebugDraw::DrawString(const b2Vec2& p, const char* string, ...)
{
}

void sDebugDraw::DrawAABB(b2AABB* aabb, sf::Color& color)
{
}