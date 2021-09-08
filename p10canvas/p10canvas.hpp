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
	/*
	void resizeEvent(QResizeEvent* e) {
		if(isVisible()) {
			//move(0,0);		
			qDebug() << geometry();
			qDebug() << e->oldSize();
			sf::View view = getDefaultView();
			// view.setRotation(20.f);
			//view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
			//view.setCenter(0,0);
			//view.setSize((float)QWidget::size().width(), (float)QWidget::size().height());
			
			view.setSize(e->size().width(), e->size().height());
			setView(view);
		}
	}
	*/
public:
	p10canvas(QWidget* parent, const QPoint& position, const sf::Vector2i resolution);
	//virtual ~p10canvas()=0;

	void pickColor(sf::Color);
	// BUG: Cannot use this method to import the new segment having different cols and rows with this object
	// 		Only use this affter constructor, this will be fix soon.
	inline void importSegment(const p10frame::FrameSegment& seg, const uint8_t* data) {
		p->importSegment(seg, data);
		// QWidget::setMinimumSize((int)p->size().x, (int)p->size().y);
	}
	inline const p10frame::FrameSegment exportSegment(std::vector<uint8_t>& data) const {
		return p->exportSegment(data);
	}
};

#endif /* __P10CANVAS_HPP__ */
