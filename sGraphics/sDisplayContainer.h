#pragma once
#include <sfml.h>
#include <set>
#include "sDisplayObject.h"

class sDisplayContainer : public sDisplayObject
{
public:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		std::set<sDisplayObject*>::iterator i;
		for(i = m_children.begin(); i != m_children.end(); ++i){
			(*i)->draw(target, states);
		}

	}


	void addChild(sDisplayObject *displayObject)
	{
		m_children.insert(displayObject);
	}
	void removeChild(sDisplayObject *displayObject)
	{
		m_children.erase(displayObject);
	}
	bool contains(sDisplayObject *displayObject)
	{
		return m_children.find(displayObject) != m_children.end();
	}

protected:



	//sf::Transform getGlobalTransform()
	//{
//
	//}


	std::set<sDisplayObject*> m_children;
	sf::Transform m_globalTransform;

private:
	

};