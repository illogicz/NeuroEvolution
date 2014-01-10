#pragma once
#include <vector>
#include "sSimpleBody.h"

using std::vector;

class sConcavePolygon : public sSimpleBody
{

private:


public:

	sConcavePolygon(float32 x = 0, float32 y = 0, float32 angle = 0)
	{
		m_bodyDef = &s_bodyDef;
		setPosition(b2Vec2(x, y));
		setAngle(angle);
	}

	void set(vector<b2Vec2> vertices)
	{
		m_vertices = vertices;
		finalizeShape();
	}

	void add(b2Vec2 vertex)
	{
		m_vertices.push_back(vertex);
	}
	void add(float32 x, float32 y)
	{
		add(b2Vec2(x, y));
	}
	void finalizeShape()
	{
		m_parts.clear();

		// todo: simplify shape, weighted to concave verts

		split(m_vertices);
		
		int n_parts = m_parts.size();
		__fixtures.resize(n_parts);
		__shapes.resize(n_parts);
		m_fixtureDefs.resize(n_parts);
		printf("%i parts \n", n_parts);


		for(int i = 0; i < n_parts; i++){
			printf("--\n");
			for(int j = 0; j < m_parts[i].size(); j++){
				printf("%f %f \n", m_parts[i][j].x, m_parts[i][j].y);
			}
			__shapes[i].Set(&m_parts[i][0], m_parts[i].size());
			__fixtures[i].shape = &__shapes[i];
			m_fixtureDefs[i] = &__fixtures[i];
		}

	}


private:

	vector<b2FixtureDef> __fixtures;
	vector<b2PolygonShape> __shapes;

	inline bool lines_intersect(b2Vec2 const& p0, b2Vec2 const& p1, b2Vec2 const& p2, b2Vec2 const& p3, b2Vec2* i = 0) {
		b2Vec2 const s1 = p1 - p0;
		b2Vec2 const s2 = p3 - p2;

		b2Vec2 const u = p0 - p2;

		float32 const ip = 1.f / (-s2.x * s1.y + s1.x * s2.y);

		float32 const s = (-s1.y * u.x + s1.x * u.y) * ip;
		float32 const t = ( s2.x * u.y - s2.y * u.x) * ip;

		if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
			if (i) *i = p0 + (t * s1);
			return true;
		}

		return false;
	}


protected:
	//m_fixtureDefs.push_back(&s_fixtureDef);
	//fixtureDef.shape = &shape;
	void split(const vector<b2Vec2> &vertices)
	{
		float32 a1, a2, da;
		int n_verts = vertices.size();
		for(int i = 0; i < n_verts; i++){

			 b2Vec2 v0 = vertices[i];
			 b2Vec2 v1 = vertices[(i + 1) % n_verts];
			 b2Vec2 v2 = vertices[(i + 2) % n_verts];

			 // check if concave
			if(b2Cross(v1 - v0, v2 - v1) > 0){

				printf("concave %f %f\n", v1.x, v1.y);
				b2Vec2 dv1 = v0 - v1;
				b2Vec2 dv2 = v2 - v1;
				dv1.Normalize();
				dv2.Normalize();

				b2Vec2 normal_sum = - dv1 - dv2;
				normal_sum.Normalize();
				 printf("normal_sum %f %f\n", normal_sum.x, normal_sum.y);
				b2Vec2 v1n = v1 + (100.f * normal_sum);

				int best_i = 0;
				float32 best_len = 100.f;
				b2Vec2 best_p;
				for(int j = 2; j < n_verts; j++){

					int i1 = (i + j) % n_verts;
					int i2 = (i + j + 1) % n_verts;
					b2Vec2 o1 = vertices[i1];
					b2Vec2 o2 = vertices[i2];
					b2Vec2 ip; // intersection point

					if(lines_intersect(o1, o2, v1, v1n, &ip)){
						float32 len = (ip - v1).LengthSquared();
						if(len < best_len){
							best_len = len;
							best_i = i1;
							best_p = ip;
						}
					}
				}
				
				vector<b2Vec2> r1, r2;
				vector<b2Vec2> *rcur = &r1;

				for(int j = 0; j <= n_verts; j++){
					int k = (i + j + 1) % n_verts;
					rcur->push_back(vertices[k]);
					if(k == best_i){
						rcur->push_back(best_p);
						rcur = &r2;
						rcur->push_back(best_p);
					}
				}
				split(r2);
				split(r1);
				
				return;
			 }
		}
		m_parts.push_back(vertices);
	}

	void addToWorld(b2World &world)
	{
		sSimpleBody::addToWorld(world);
	}
	vector<vector<b2Vec2>> m_parts;
	vector<b2Vec2> m_vertices;

	b2BodyDef s_bodyDef;
};