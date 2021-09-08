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
	color |= (led[0].color.r>200)<<0;
	color |= (led[0].color.g>200)<<1;
	color |= (led[0].color.b>200)<<2;
	return color;
}
void DotLed::set3bitColor(uint8_t color) {
	setColor(sf::Color(
		((color & p10frame::RED)==p10frame::RED)*255,
		((color & p10frame::GREEN)==p10frame::GREEN)*255,
		((color & p10frame::BLUE)==p10frame::BLUE)*255	
	));
}

p10::p10 (sf::Vector2f position, sf::Vector2i resolution, float bitsize, float space):
m_pos(position), m_res(resolution), mbitsize(bitsize), mspace(space)
{
	p10frame::FrameSegment seg = {(uint16_t)resolution.x, (uint16_t)resolution.y};
	//std::vector<uint8_t> u(resolution.x*resolution.y, p10frame::BLUE);
	importSegment(seg);
}
p10::~p10() {
	cleanupPixels();
}
void p10::DrawTo(sf::RenderTarget& window) {
	for(auto i: pixels) {
		for(auto j: i) {
			j->DrawTo(window);
		}
	}
}
void p10::cleanupPixels() {	
	for(auto& i: pixels) {
		i.resize(m_res.x);
		for(auto& j: i){
			delete j;
		}
		i.clear();
	}
	m_res = {0,0};
}

DotLed* p10::getDotBy(sf::Vector2f position)	 {
	float x = position.x - m_pos.x;
	float y = position.y - m_pos.y;
	if(x < 0 || y < 0) return 0;
	if(x >= msize.x || y >= msize.y) return 0;
	float pixperled = mbitsize + mspace;
	DotLed* b = pixels[(int)(y/(pixperled))] [(int)(x/pixperled)];

	return b;
}

void p10::importSegment(const p10frame::FrameSegment& seg, const uint8_t* data) {
	cleanupPixels();
	m_res.x = seg.frameSize.nCols;
	m_res.y = seg.frameSize.nRows;
	msize = {m_res.x*(mbitsize+mspace), m_res.y*(mbitsize+mspace)};
	pixels.resize(m_res.x * m_res.y);
	int k = 0;
	for(int i(0); i<m_res.y; i++) {
		pixels[i].resize(m_res.x);
		for(int j(0); j<m_res.x; j++){
			pixels[i][j] = new DotLed(m_pos+sf::Vector2f((mbitsize+mspace)*j,(mbitsize+mspace)*i),mbitsize);
			if(data)pixels[i][j]->set3bitColor(data[k++]);
		}
	}
}

const p10frame::FrameSegment p10::exportSegment(std::vector<uint8_t>& data) const { // TODO: update this
	p10frame::FrameSegment seg;
	seg.frameSize.nCols = m_res.x;
	seg.frameSize.nRows = m_res.y;
	data.resize(seg.frameSize.nPixels());
	int k(0);
	for(int i(0); i<m_res.y; i++) {
		for(int j(0); j<m_res.x; j++){
			data[k++] = pixels[i][j]->get3bitColor();
		}
	}
	return seg;
}