#pragma once
#include <sfml.h>
#include <string>
using std::string;

class sText
{

public:

	static void loadFont(string fileName)
	{

		if (!m_font.loadFromFile("consola.ttf")){
			printf("error loading font \n");
			return;
		}
		m_textField.setFont(m_font);
		m_fontLoaded = true;
	}

	static void drawText(sf::RenderTarget &target, 
		          const string &text, 
		          float x, float y, 
				  unsigned int size = 16, 
				  const sf::Color &color = sf::Color::White,
				  bool center = false)
	{
		if(m_fontLoaded){

			m_textField.setCharacterSize(size);
			//m_textField.setColor(color);
			m_textField.setString(text);
			if(center){
				m_textField.setPosition(x - m_textField.getLocalBounds().width / 2, y);
			} else {
				m_textField.setPosition(x, y);
			}
			target.draw(m_textField);
		}
	}

private:

	static sf::Text m_textField;
	static bool m_fontLoaded;
	static sf::Font m_font;


};
sf::Text sText::m_textField;
sf::Font sText::m_font;
bool sText::m_fontLoaded = false;