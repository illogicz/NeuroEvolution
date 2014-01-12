#ifndef RENDER_H
#define RENDER_H

#include <Box2D/Box2D.h>
#include <sfml.h>


struct b2AABB;

// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.
// 
class sDebugDraw : public b2Draw
{
public:

	sDebugDraw(sf::RenderTarget &renderTarget) : target(renderTarget)
	{
		triangles.setPrimitiveType(sf::PrimitiveType::Triangles);
		lines.setPrimitiveType(sf::PrimitiveType::Lines);
	}


	void prepare()
	{
		triangles_index = lines_index = 0;
	}

	void finalize()
	{
		// draw triangles
		triangles.resize(triangles_index);
		target.draw(triangles, states);

		// draw lines
		lines.resize(lines_index);
		target.draw(lines, states);
	}


	void setTransform(sf::Transform transform)
	{
		states.transform = transform;
	}

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	void DrawTransform(const b2Transform& xf);

    void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);

    void DrawString(int x, int y, const char* string, ...); 

    void DrawString(const b2Vec2& p, const char* string, ...);

    void DrawAABB(b2AABB* aabb, const b2Color& color);

private:

	void allocate(sf::VertexArray &va, int i, int n);


	sf::RenderStates states;
	sf::RenderTarget &target;
	sf::VertexArray triangles;
	sf::VertexArray lines;

	int triangles_index;
	int lines_index;

	//sf::View *view;
};


#endif