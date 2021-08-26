#ifndef __P10SPRITES_HPP__
#define __P10SPRITES_HPP__
/**
 * @file		p10sprites.hpp
 * @author		github.com/kienvo (kienvo@kienlab.com)
 * @brief 		sfml stuff for drawing pixel header file
 * @version		0.1
 * @date		Aug-16-2021
 * 
 * @copyright	Copyright (c) 2021 by kienvo@kienlab.com
 * 
 */
#include <SFML/Graphics.hpp>
#include <p10frame/p10frame.hpp>

//TODO: add color
class DotLed {
private:
	sf::Vector2f m_pos;
	sf::VertexArray led;
public:
	DotLed(sf::Vector2f position, float size);
	void setPosition(sf::Vector2f position);
	void DrawTo(sf::RenderTarget& window);
	void setColor(sf::Color color);
	uint8_t get3bitColor();
};

class byteLed {
private:
	DotLed* leds[8];
	sf::Vector2f m_pos;
public:
	byteLed(sf::Vector2f position, float bitsize, float space);
	void DrawTo(sf::RenderTarget& window);
	DotLed* getDotBy(int i);
	uint8_t getByteVal();
};

class p10 {
private:
	sf::Vector2f m_pos;
	std::vector<std::vector<byteLed*>> bytes;
	sf::Vector2f m_res;
	sf::Vector2f msize;
	float mbitsize;
	float mspace;
public:
	p10 (sf::Vector2f position, sf::Vector2i resolution, float bitsize, float space);
	inline sf::Vector2f size() {
		return msize;
	}
	void DrawTo(sf::RenderTarget& window);
	DotLed* getDotBy(sf::Vector2f position);
};
#endif //__P10SPRITES_HPP__