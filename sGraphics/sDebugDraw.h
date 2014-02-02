#ifndef RENDER_H
#define RENDER_H

#include <Box2D/Box2D.h>
#include <sfml.h>
#include "../sPhysics/sWorld.h"

struct b2AABB;



class sDebugDraw// : public b2Draw
{
public:

	sDebugDraw()
	{
		triangles.setPrimitiveType(sf::PrimitiveType::Triangles);
		lines.setPrimitiveType(sf::PrimitiveType::Lines);
		clear();
	}

	void clear()
	{
		triangles_index = lines_index = 0;
	}

	void setTarget(sf::RenderTarget *target)
	{
		m_target = target;
	}

	enum
	{
		e_shapeBit				= 0x0001,	///< draw shapes
		e_jointBit				= 0x0002,	///< draw joint connections
		e_aabbBit				= 0x0004,	///< draw axis aligned bounding boxes
		e_pairBit				= 0x0008,	///< draw broad-phase pairs
		e_centerOfMassBit		= 0x0010	///< draw center of mass frame
	};

	void SetFlags(uint32 flags)
	{
		m_drawFlags = flags;
	}

	uint32 GetFlags() const
	{
		return m_drawFlags;
	}

	void setTransform(sf::Transform transform)
	{
		states.transform = transform;
	}

	void setView(sf::View v)
	{
		view_size = 0.5f * states.transform.getInverse().transformPoint(v.getSize());
		view_center = states.transform.getInverse().transformPoint(v.getCenter());
	}

	void addTriangle(const b2Vec2 &v1, const b2Vec2 &v2, const b2Vec2 &v3, sf::Color &c);
	void addLine(const b2Vec2 &v1, const b2Vec2 &v2, sf::Color &c);
	void DrawDebugData(sWorld &world);
	void DrawGrid(const b2AABB &aabb);       
	void DrawShape(b2Fixture* fixture, const b2Transform& xf, sf::Color& color);
	void DrawJoint(b2Joint* joint);
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, sf::Color& color);
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, sf::Color& color);
	void DrawCircle(const b2Vec2& center, float32 radius, sf::Color& color);
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, sf::Color& color);
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, sf::Color& color);
	void DrawTransform(const b2Transform& xf);
    void DrawPoint(const b2Vec2& p, float32 size, sf::Color& color);
    void DrawString(int x, int y, const char* string, ...); 
    void DrawString(const b2Vec2& p, const char* string, ...);
    void DrawAABB(b2AABB* aabb, sf::Color& color);

private:

	void allocate(sf::VertexArray &va, int i, int n);

	sf::RenderTarget *m_target;
	sf::Vector2f view_center;
	sf::Vector2f view_size;
	sf::RenderStates states;
	sf::VertexArray triangles;
	sf::VertexArray lines;

	int triangles_index;
	int lines_index;
	uint32 m_drawFlags;

	class AABBQueryCallback : public b2QueryCallback
	{
	public:
		bool ReportFixture(b2Fixture* fixture){
			fixtures.push_back(fixture);
			return true;
		}
		vector<b2Fixture*> fixtures;
	};
	
	//sf::View *view;
};


#endif