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
				  float size = 16, 
				  const sf::Color &color = sf::Color::White)
	{
		if(m_fontLoaded){
			m_textField.setCharacterSize(size);
			m_textField.setPosition(x, y);
			m_textField.setColor(color);
			m_textField.setString(text);
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