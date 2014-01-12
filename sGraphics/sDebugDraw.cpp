#include "sDebugDraw.h"




void sDebugDraw::allocate(sf::VertexArray &va, int i, int n)
{
	if(i + n > va.getVertexCount()){
		va.resize(i + n);
	}
}




void sDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	allocate(lines, lines_index, (vertexCount-1)*2);
	sf::Color c = sf::Color(color.r*0xFF, color.g*0xFF, color.b*0xFF, 0xFF);
	for(int i=0; i < vertexCount-1; i++){
		lines[lines_index].position.x = vertices[i].x;
		lines[lines_index].position.y = vertices[i].y;
		lines[lines_index].color = c;
		lines_index++;
		lines[lines_index].position.x = vertices[i+1].x;
		lines[lines_index].position.y = vertices[i+1].y;
		lines[lines_index].color = c;
		lines_index++;
	}
}

void sDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	sf::Color c = sf::Color(color.r*0xFF, color.g*0xFF, color.b*0xFF, 0x60);

	allocate(triangles, triangles_index, (vertexCount-2)*3);
	for(int i=0; i < vertexCount-2; i++){
		triangles[triangles_index].color = c;
		triangles[triangles_index].position.x = vertices[0].x;
		triangles[triangles_index].position.y = vertices[0].y;
		triangles_index++;
		triangles[triangles_index].color = c;
		triangles[triangles_index].position.x = vertices[i+1].x;
		triangles[triangles_index].position.y = vertices[i+1].y;
		triangles_index++;
		triangles[triangles_index].color = c;
		triangles[triangles_index].position.x = vertices[i+2].x;
		triangles[triangles_index].position.y = vertices[i+2].y;
		triangles_index++;
	}

	allocate(lines, lines_index, vertexCount*2);
	c.a = 0xFF;
	for(int i=0; i < vertexCount; i++){
		lines[lines_index].position.x = vertices[i].x;
		lines[lines_index].position.y = vertices[i].y;
		lines[lines_index].color = c;
		lines_index++;
		lines[lines_index].position.x = vertices[(i+1) % vertexCount].x;
		lines[lines_index].position.y = vertices[(i+1) % vertexCount].y;
		lines[lines_index].color = c;
		lines_index++;
	}

}

void sDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	sf::CircleShape circle(radius, 64);
	circle.setPosition(center.x, center.y);
	circle.setFillColor(sf::Color(color.r*0xFF, color.g*0xFF, color.b*0xFF, 0));
	circle.setOutlineColor(sf::Color(color.r*0xFF, color.g*0xFF, color.b*0xFF, 0xFF));
	target.draw(circle, states);
}

void sDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{


	sf::CircleShape circle(radius, 64);
	circle.setPosition(center.x-radius, center.y-radius);
	circle.setFillColor(sf::Color(color.r*0xFF, color.g*0xFF, color.b*0xFF, 0x60));
	circle.setOutlineThickness(1.f/20);
	circle.setOutlineColor(sf::Color(color.r*0xFF, color.g*0xFF, color.b*0xFF, 0xFF));
	target.draw(circle, states);

	allocate(lines, lines_index, 2);
	lines[lines_index].position.x = center.x;
	lines[lines_index].position.y = center.y;
	lines[lines_index].color = sf::Color(color.r*0xFF, color.g*0xFF, color.b*0xFF, 0xFF);
	lines_index++;
	lines[lines_index].position.x = center.x + axis.x * radius;
	lines[lines_index].position.y = center.y + axis.y * radius;
	lines[lines_index].color = sf::Color(color.r*0xFF, color.g*0xFF, color.b*0xFF, 0xFF);
	lines_index++;

}

void sDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	sf::Color c = sf::Color(color.r*0xFF, color.g*0xFF, color.b*0xFF, 0xFF);

	allocate(lines, lines_index, 2);

	lines[lines_index].position.x = p1.x;
	lines[lines_index].position.y = p1.y;
	lines[lines_index].color = c;
	lines_index++;
	lines[lines_index].position.x = p2.x;
	lines[lines_index].position.y = p2.y;
	lines[lines_index].color = c;
	lines_index++;
}

void sDebugDraw::DrawTransform(const b2Transform& xf)
{
}

void sDebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
}

void sDebugDraw::DrawString(int x, int y, const char* string, ...)
{
}

void sDebugDraw::DrawString(const b2Vec2& p, const char* string, ...)
{
}

void sDebugDraw::DrawAABB(b2AABB* aabb, const b2Color& color)
{
}