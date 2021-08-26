/**
 * @file		p10sprites.hpp
 * @author		github.com/kienvo (kienvo@kienlab.com)
 * @brief 		sfml stuff for drawing pixel source file
 * @version		0.1
 * @date		Aug-16-2021
 * 
 * @copyright	Copyright (c) 2021 by kienvo@kienlab.com
 * 
 */

#include "p10sprites/p10sprites.hpp"

DotLed::DotLed(sf::Vector2f position, float size) {
	m_pos = position;
	led.setPrimitiveType(sf::Quads);
	led.append(sf::Vertex(m_pos+sf::Vector2f(0,0), sf::Color::Black));
	led.append(sf::Vertex(m_pos+sf::Vector2f(0,size), sf::Color::Black));
	led.append(sf::Vertex(m_pos+sf::Vector2f(size,size), sf::Color::Black));
	led.append(sf::Vertex(m_pos+sf::Vector2f(size,0), sf::Color::Black));
}
void DotLed::setPosition(sf::Vector2f position) {
	m_pos = position;
}
void DotLed::DrawTo(sf::RenderTarget& window) {
	window.draw(led);
}
void DotLed::setColor(sf::Color color) {
	for(uint i=0; i<led.getVertexCount(); i++) {
		led[i].color = color;
	}
}
uint8_t DotLed::get3bitColor() {
	uint8_t color(0);
	color |= (led[0].color.r==255)<<1;
	color |= (led[0].color.g==255)<<2;
	color |= (led[0].color.b==255)<<3;
}



byteLed::byteLed(sf::Vector2f position, float bitsize, float space) {
	m_pos = position;
	for(int i=0; i<8; i++) {
		leds[i] = new DotLed(m_pos+sf::Vector2f(0,i*bitsize+space*i), bitsize);
	}
}
void byteLed::DrawTo(sf::RenderTarget& window) {
	for(int i=0; i<8; i++) {
		leds[i]->DrawTo(window);
	}
}
DotLed* byteLed::getDotBy(int i) {
	return leds[i];
}


p10::p10 (sf::Vector2f position, sf::Vector2i resolution, float bitsize, float space):
m_pos(position), m_res(resolution), mbitsize(bitsize), mspace(space)
{
	msize = {(bitsize+space)*resolution.x,(bitsize+space)*resolution.y*8};
	bytes.resize(resolution.y);
	for(int i(0); i<resolution.y; i++) {
		bytes[i].resize(resolution.x);
		for(int j(0); j<resolution.x; j++){
			bytes[i][j] = new byteLed(position+sf::Vector2f((bitsize+space)*j,(bitsize+space)*8*i),bitsize, space);
		}
	}
}
void p10::DrawTo(sf::RenderTarget& window) {
	for(std::vector<byteLed*> i: bytes) {
		for(byteLed* j: i) {
			j->DrawTo(window);
		}
	}
}
DotLed* p10::getDotBy(sf::Vector2f position)	 {
	float x = position.x - m_pos.x;
	float y = position.y - m_pos.y;
	if(x < 0 || y < 0) return 0;
	if(x >= msize.x || y >= msize.y) return 0;
	float pixperled = mbitsize + mspace;
	byteLed* b = bytes[(int)(y/(pixperled*8))] [(int)(x/pixperled)];

	return b->getDotBy( (int)(y/pixperled)%8 );
}
