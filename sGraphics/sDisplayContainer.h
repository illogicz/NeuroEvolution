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
		std::set<sf::Drawable*>::iterator i;
		for(i = m_children.begin(); i != m_children.end(); ++i){
			//(*i)->draw(target, states);
		}

	}


	void addChild(sf::Drawable *displayObject)
	{
		m_children.insert(displayObject);
	}
	void removeChild(sf::Drawable *displayObject)
	{
		m_children.erase(displayObject);
	}
	bool contains(sf::Drawable *displayObject)
	{
		return m_children.find(displayObject) != m_children.end();
	}

protected:



	//sf::Transform getGlobalTransform()
	//{
//
	//}


	std::set<sf::Drawable*> m_children;
	sf::Transform m_globalTransform;

private:
	

};