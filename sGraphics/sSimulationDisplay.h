#pragma once
#include <sfml.h>
#include "../sGraphics/sDebugDraw.h"
#include "../sEvolution/sSimulation.h"


class sSimulationDisplay
{

public:

	sSimulationDisplay()
	{

		m_debugDraw.SetFlags(sDebugDraw::e_shapeBit | sDebugDraw::e_jointBit);
		setSize(640, 480);
		m_focusLeader = true;
	}


	void draw(sSimulation &simulation, sf::RenderTarget *target, sf::RenderStates states)
	{
		float scale = simulation.renderScale;

		b2Vec2 wp = b2Vec2(float(m_view.getCenter().x) / scale, float(m_view.getCenter().y) / scale);
		b2Vec2 p = m_focusLeader ? simulation.leader->getPosition() : simulation.trailer->getPosition();
		p.y = wp.y + 0.1f * (p.y - wp.y);
		float32 dx = p.x - wp.x;// + simulation.leader->getVelocity().x / scale;
		//if(abs(dx) > 5) dx *= abs(dx) / 5;
		float32 f = dx / 50;
		f = f > 1 ? 1 : f;
		p.x = wp.x + f * dx;
		m_view.setCenter(p.x * scale, p.y * scale);	


		sf::Transform trans;
		trans.scale(scale, scale);	
		target->setView(m_view);

		m_debugDraw.setTransform(trans);
		m_debugDraw.setView(m_view);
		m_debugDraw.prepare(target);
		m_debugDraw.DrawDebugData(simulation.world);
		m_debugDraw.finalize();

		// Reset view to default
		target->setView(target->getDefaultView());

	}

	void setSize(float width, float height)
	{
		m_view.setSize(width, height);
	}

	void setCenter(float x, float y)
	{
		m_view.setCenter(x, y);
	}

	void toggleFocus()
	{
		m_focusLeader = !m_focusLeader;
	}



private:



	void drawStatistics()
	{

	}

	bool m_focusLeader;
	sf::View m_view;
	sDebugDraw m_debugDraw;


};