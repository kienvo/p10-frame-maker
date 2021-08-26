/**
 * @file		p10canvas.hpp
 * @author		github.com/kienvo (kienvo@kienlab.com)
 * @brief 		pixel drawing widget header file
 * @version		0.1
 * @date		Aug-16-2021
 * 
 * @copyright	Copyright (c) 2021 by kienvo@kienlab.com
 * 
 */
#ifndef __P10CANVAS_HPP__
#define __P10CANVAS_HPP__
#include <iostream>
#include "qsfml/qsfml.h"
#include "p10sprites/p10sprites.hpp"
#include "p10frame/p10frame.hpp"

class p10canvas :  public QSFML
{

private:
	void OnInit();
	void OnUpdate();
	void DrawColor(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event); 
	void mouseMoveEvent(QMouseEvent* event);
	bool LeftBtnState;
	p10* p;
	sf::Color colorState;
public:
	p10canvas(QWidget* parent, const QPoint& position, const sf::Vector2i resolution);
	//virtual ~p10canvas()=0;

	void pickColor(sf::Color);
};

#endif /* __P10CANVAS_HPP__ */
