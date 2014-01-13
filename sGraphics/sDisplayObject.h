#pragma once
#include <sfml.h>

class sDisplayContainer;

class sDisplayObject : public sf::Transformable, public sf::Drawable
{
public:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		sf::CircleShape circle;
		circle.setRadius(20);
		circle.setFillColor(sf::Color::White);
		circle.setPosition(0,0);
		circle.setOrigin(20,20);
		target.draw(circle, states);
	}


protected:

	bool m_hasParent;
	sDisplayContainer *m_parent;

private:
	

};