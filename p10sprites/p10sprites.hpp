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
	void set3bitColor(uint8_t color);
};


class p10
{
private:
	sf::Vector2f m_pos;
	std::vector<std::vector<DotLed*>> pixels;
	sf::Vector2f m_res;
	sf::Vector2f msize;
	float mbitsize;
	float mspace;
	void cleanupPixels();
public:
	p10 (sf::Vector2f position, sf::Vector2i resolution, float bitsize, float space);
	/**
	 * @brief 		return the size of window in pixel
	 * 
	 * @return		sf::Vector2f 
	 */
	inline sf::Vector2f size() {
		return msize;
	}
	~p10();
	void DrawTo(sf::RenderTarget& window);
	/**
	 * @brief 		Get dot by pixel position
	 * 
	 * @param		position mouse relative position with m_pos
	 * @return		DotLed* 
	 */
	DotLed* getDotBy(sf::Vector2f position);
	void importSegment(const p10frame::FrameSegment& seg, const uint8_t *data=nullptr);
	/**
	 * @brief 		export frame data
	 * 
	 * @param		data pointer allocated memory, remember to allocate an array before call this function to get data
	 * @return		const p10frame::FrameSegment frame description
	 */
	const p10frame::FrameSegment exportSegment(std::vector<uint8_t>& data) const;
};
#endif //__P10SPRITES_HPP__