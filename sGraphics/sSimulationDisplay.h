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


	void draw(sf::RenderTarget *target, sf::RenderStates states)
	{
		float scale = m_simulation->renderScale;


		if(!m_lockPosition){

			b2Vec2 wp = b2Vec2(float(m_view.getCenter().x) / scale, float(m_view.getCenter().y) / scale);
			b2Vec2 p = m_focusLeader ? m_simulation->leader->getPosition() : m_simulation->trailer->getPosition();
			p.y = wp.y + 0.1f * (p.y - wp.y);
			float32 dx = p.x - wp.x;// + simulation.leader->getVelocity().x / scale;
			//if(abs(dx) > 5) dx *= abs(dx) / 5;
			float32 f = (abs(dx)+50) / 100;
			f = f > 1 ? 1 : f;
			p.x = wp.x + f * dx;

			setCenter(p.x * scale, p.y * scale);	
		}

		sf::Transform trans;
		trans.scale(scale, scale);	
		target->setView(m_view);

		m_debugDraw.setTransform(trans);
		m_debugDraw.setView(m_view);
		m_debugDraw.prepare(target);
		m_debugDraw.DrawDebugData(m_simulation->world);
		m_debugDraw.finalize();

		// Reset view to default
		// target->setView(target->getDefaultView());

	}

	void setSize(float width, float height)
	{
		m_view.setSize(width, height);
	}

	void setCenter(float x, float y)
	{
		m_view.setCenter(x, y);
	}

	//b2Vec2 getCenter()
	//{
	//	return m_view.getCenter();
	//}

	void toggleFocus()
	{
		m_focusLeader = !m_focusLeader;
	}

	void setSimulation(sSimulation *simulation)
	{
		m_simulation = simulation;
		m_mouseJoint.setBodyA(simulation->world.getGroundBody());
		m_simulation->world.add(&m_mouseJoint);
	}


	void mousePressed(b2Vec2 p)
	{
		if(m_mouseJoint.pressed(screenToWorld(p))){
			m_lockPosition = true;
		}
	}
	void mouseReleased(b2Vec2 p)
	{
		m_mouseJoint.released();
		m_lockPosition = false;
	}
	void mouseMoved(b2Vec2 p)
	{
		m_mouseJoint.setPosition(screenToWorld(p));
	}


	b2Vec2 screenToWorld(b2Vec2 point)
	{
		point.x -= m_view.getSize().x / 2;
		point.y -= m_view.getSize().y / 2;
		point.x += m_view.getCenter().x;
		point.y += m_view.getCenter().y;
		point.x /= m_simulation->renderScale;
		point.y /= m_simulation->renderScale;
		return point;

	}

	b2Vec2 worldToScreen(b2Vec2 point)
	{

	}


private:



	void drawStatistics()
	{

	}


	sMouseJoint m_mouseJoint;
	sSimulation *m_simulation;
	bool m_focusLeader;
	bool m_lockPosition;
	sf::View m_view;
	sDebugDraw m_debugDraw;


};