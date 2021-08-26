/**
 * @file		p10canvas.cpp
 * @author		github.com/kienvo (kienvo@kienlab.com)
 * @brief 		pixel drawing widget source file
 * @version		0.1
 * @date		Aug-16-2021
 * 
 * @copyright	Copyright (c) 2021 by kienvo@kienlab.com
 * 
 */
#include "p10canvas/p10canvas.hpp"

p10canvas::p10canvas(QWidget* parent, const QPoint& position,
	 const sf::Vector2i resolution) :
QSFML(parent, position)
{
	float bitsize = 5.f;
	float spacing = 1.f;
	p = new p10({spacing,spacing},resolution,bitsize,spacing);
	QWidget::resize((int)p->size().x+spacing, (int)p->size().y+spacing);
	
	sf::View view = RenderTarget::getDefaultView();
	view.setSize(p->size().x, p->size().y);
	RenderTarget::setView(view);
}

void p10canvas:: pickColor(sf::Color color){
	colorState = color;
	//std::cerr << "button click" << std::endl;
}

void p10canvas:: OnInit() {
	//QColor color = QColorDialog::getColor(Qt::yellow,this);
	//color.convertTo()
}
void p10canvas:: OnUpdate() { 
	RenderWindow::clear(sf::Color(127,127,127));
	p->DrawTo(*this);
	RenderWindow::display();
}
void p10canvas::DrawColor(QMouseEvent* event) {
	DotLed *t = p->getDotBy(mapPixelToCoords({event->x(),event->y()})); // TODO: Zoomming is not used
	if(event->modifiers() & Qt::AltModifier) {
		if(t) t->setColor(sf::Color::Black); // TODO: remove led
		return;
	}
	if(t) t->setColor(colorState); // TODO: set color led
}
void p10canvas::mousePressEvent(QMouseEvent* event) {
	if(event->button() == Qt::LeftButton){
		LeftBtnState = true;
		DrawColor(event);
	} else {

	}
}
void p10canvas::mouseReleaseEvent(QMouseEvent* event) {
	if(event->button() == Qt::LeftButton){
		LeftBtnState = false;
	} else {

	}		
}
void p10canvas::mouseMoveEvent(QMouseEvent* event) {
	// qDebug() << event;
	if(event->buttons() & Qt::LeftButton) DrawColor(event);
}
