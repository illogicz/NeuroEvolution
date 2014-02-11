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
		m_focusChanged = false;
		m_focusRank = 0;
		m_lockPosition = false;
		m_lockFocus = false;
		setCenter(0,0);
	}


	void draw(sf::RenderTarget *target, sf::RenderStates states)
	{
		float scale;
		if(m_simulation->staticView){
			scale = m_simulation->renderScale;
		} else {
			scale = m_simulation->renderScale * m_simulation->zoomScale;
		}
		

		updateFocus();

		if(!m_simulation->staticView){

			if(!m_lockPosition){

				b2Vec2 wp = b2Vec2(float(m_view.getCenter().x) / scale, float(m_view.getCenter().y) / scale);
				//b2Vec2 p = m_focusLeader ? m_simulation->leader->getPosition() : m_simulation->trailer->getPosition();
				b2Vec2 p = m_focusTarget->getPosition();
				p.y = wp.y + 0.1f * (p.y - wp.y);
				float32 dx = p.x - wp.x;// + simulation.leader->getVelocity().x / scale;
				//if(abs(dx) > 5) dx *= abs(dx) / 5;
				float32 f = (abs(dx)+5.f) / 50;
				f = f > 0.8 ? 0.8 : f;
				p.x = wp.x + f * dx;

				setCenter(p.x * scale, p.y * scale);	
			}

			//if(m_focusChanged || !m_lockFocus){


				//for(int i = 0; i < m_simulation->population.size(); i++){
				//	float a = 1.f - abs(getFocusRank() - i) / 5.f;
				//	if(a < 0.05)a = 0.05;
				//	m_simulation->population[i]->setAlpha(a);
				//}

			//}
		} else {
			
		}



		sf::Transform trans;
		trans.scale(scale, scale);	
		target->setView(m_view);

		m_debugDraw.setTransform(trans);
		m_debugDraw.setView(m_view);
		for(int i = 0; i < m_simulation->worlds.size(); i++){
			m_debugDraw.DrawDebugData(m_simulation->worlds[i]);
		}
		m_debugDraw.draw(*target);
		// Reset view to default
		// target->setView(target->getDefaultView());

	}

	void clearDebugDraw()
	{
		m_debugDraw.clear();
	}

	void setSize(float width, float height)
	{
		m_view.setSize(width, height);
	}

	void setCenter(float x, float y)
	{
		m_view.setCenter(x, y);
	}

	void setFocusRank(int rank)
	{
		if(rank < 0)rank = 0;
		if(rank >= m_simulation->population.size()) 
			rank = m_simulation->population.size() - 1;
		//if(m_focusRank != rank){
			m_focusRank = rank;
			m_focusChanged = true;
		//}
		updateFocus();

	}

	int getFocusRank()
	{
		if(m_lockFocus){
			return m_focusTarget->getRank();
		} else {
			return m_focusRank;
		}
	}

	sPhenotype *getFocusTarget()
	{
		return m_focusTarget;
	}

	b2Vec2 getFocusPosition()
	{
		return getFocusTarget()->getPosition();
	}

	void focusFirst()
	{
		setFocusRank(0);
	}
	void focusLast()
	{
		setFocusRank(m_simulation->population.size() - 1);
	}
	void focusNext()
	{
		setFocusRank(getFocusRank() - 1);
	}
	void focusPrev()
	{
		printf("focus prev\n");
		setFocusRank(getFocusRank() + 1);
	}

	void toggleLockFocus()
	{
		m_lockFocus = !m_lockFocus;
		printf("toggle lock %i \n", m_focusRank);
		if(m_lockFocus){
			m_focusTarget = m_simulation->population[m_focusRank];
		} else {
			setFocusRank(m_focusTarget->getRank());		
		}
	}
	bool getLockFocus()
	{
		return m_lockFocus;
	}

	void toggleStaticView()
	{
		m_simulation->staticView = !m_simulation->staticView;
		if(m_simulation->staticView){
			setCenter(m_simulation->staticViewPosition.x, m_simulation->staticViewPosition.y);
			for(int i = 0; i < m_simulation->population.size(); i++){
				m_simulation->population[i]->setAlpha(1);
			}
		}
	}

	void setSimulation(sSimulation *simulation)
	{
		m_simulation = simulation;
		m_mouseJoint.setBodyA(simulation->worlds[0].getGroundBody());
		m_simulation->worlds[0].add(&m_mouseJoint);
		m_simulation->worlds[0].setDebugDraw(&m_debugDraw);
		setFocusRank(0);
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
		point.x /= m_simulation->renderScale * m_simulation->zoomScale;
		point.y /= m_simulation->renderScale * m_simulation->zoomScale;
		return point;

	}

	b2Vec2 worldToScreen(b2Vec2 point)
	{

	}


private:

	void updateFocus()
	{
		if(m_focusTarget){
			m_focusTarget->setIsFocus(false);
		}
		if(!m_lockFocus || m_focusChanged){
			m_focusTarget = m_simulation->population[m_focusRank];
		}
		m_focusChanged = false;
		m_focusTarget->setIsFocus(true);
	}

	void drawStatistics()
	{

	}


	sMouseJoint m_mouseJoint;
	sSimulation *m_simulation;
	sPhenotype *m_focusTarget;
	int m_focusRank;
	bool m_focusChanged;
	bool m_lockFocus;
	bool m_lockPosition;
	sf::View m_view;
	sDebugDraw m_debugDraw;


};