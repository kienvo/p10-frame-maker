/**
 * \file    main.cpp
 * \brief   A tool for creating frame file using led p10 module
 * \author  kienvo (kienlab.com)
 * \version 0.1
 * \date    Aug-12-2021
 * \copyright Copyright (c) 2021 by kienvo
 */

#include <iostream>
#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QBoxLayout>
#include <QFrame>
#include <qsfml/qsfml.h>
#include <assert.h>
#include <QMainWindow>
#include "p10canvas/p10canvas.hpp"


const int window_width = 700;
const int window_height = 400;
struct Setting {
	sf::Vector2i resolution;
};

class ColorPicker: 
public QRadioButton
{
	QColor _color;
	QColor _backgr;
	void drawSelectedBackground(QPainter& painter) {
		painter.setPen(QPen(Qt::gray));
		painter.setBrush(QBrush(_backgr));
		QPoint p[4] = {
			{0,0},
			{width(), 0},
			{width(), height()},
			{0, height()}
		};
		painter.drawConvexPolygon(p, 4);
	}
	void drawButton(QPainter& painter) {
		painter.setPen(QPen(Qt::gray));
		painter.setBrush(QBrush(_color));
		painter.drawEllipse({width()/2,height()/2},width()/2-2,height()/2-2);		
	}
	void paintEvent(QPaintEvent* e) {
		QPainter painter(this);
		if(isChecked())drawSelectedBackground(painter);
		drawButton(painter);
	}
	void showEvent(QShowEvent* e) {
		setCheckable(true); 
	}
	void mousePressEvent(QMouseEvent* e) {
		if(e->button() == Qt::LeftButton) {
			click();
		}
	}

public:
	//using QAbstractButton::QAbstractButton;
	ColorPicker(QWidget* parent,QColor color = Qt::yellow, QColor selectedBackground = Qt::gray) {
		setParent(parent);
		_color = color;
		_backgr =selectedBackground;
	}
	inline void setColor(QColor color) {
		_color = color;
	}
	inline void setSelectedBackgrColor(QColor color) {
		_backgr = color;
	}
};


class MainWindow:
public QMainWindow
{
private:
	const QColor qBtnColor[8] = {
		Qt::black,
		Qt::white,
		Qt::blue,
		Qt::cyan,
		Qt::green,
		Qt::yellow,
		Qt::red,
		Qt::magenta
	};
	const sf::Color sfBtnColor[8] = {
		sf::Color::Black,
		sf::Color::White,
		sf::Color::Blue,
		sf::Color::Cyan,
		sf::Color::Green,
		sf::Color::Yellow,
		sf::Color::Red,
		sf::Color::Magenta
	};
	ColorPicker* mbtns[8];
	QGridLayout* _layout;
	Setting setting;

	void createToolbar() {
		QToolBar* colorpicker = addToolBar(tr("abc"));
        //colorpicker->addAction(newAct);
		for(int i(0); i<8; i++) {
			mbtns[i] = new ColorPicker(nullptr,qBtnColor[i]);
			mbtns[i]->setFixedSize(20,20);
			colorpicker->addWidget(mbtns[i]);
		}
	}
	void createMenubar() {
		menuBar()->addMenu("File");
	}
	void createStatusbar() {
		statusBar()->showMessage(tr("Ready"));
	}
	void popupConfigWindow() {
		std::pair<QString, int> p10cols[] = {
			{"32", 32},
			{"64", 64},
			{"96", 96},
			{"128", 128},
			{"160", 160},
			{"192", 192}
		};
		std::pair<QString, int> p10rows[] = {
			{"16", 16},
			{"32", 32},
			{"48", 48},
			{"64", 64},
			{"80", 80},
			{"96", 96},
			{"112", 112}
		};
		QDialog d(this);
		QGridLayout layout(&d);
		QPushButton done("Done");
		QComboBox col;
		QComboBox row;
		
		for(auto i: p10cols) {
			col.addItem(i.first);
		}
		for(auto i: p10rows) {
			row.addItem(i.first);
		}
		QLabel collabel("Number of colums");
		QLabel rowlabel("Number of rows");
		layout.addWidget(&collabel,0,0);
		layout.addWidget(&col, 0, 1);
		layout.addWidget(&rowlabel,1,0);
		layout.addWidget(&row, 1, 1);
		layout.addWidget(&done, 2, 0, 2, 1);
		QObject::connect(&done, &QPushButton::clicked, [&](){
			setting.resolution.y = (row.currentIndex()+1)*2;
			setting.resolution.x = (col.currentIndex()+1)*32;
			d.accept();
		});
		d.setWindowTitle("Please set resolution");
		d.setFixedSize(d.sizeHint());
		d.exec();
		std::cerr << "resolution " << setting.resolution.x << " " << setting.resolution.y << std::endl;
	}
public:

	//using QMainWindow::QMainWindow;
	MainWindow(){
		popupConfigWindow();
		p10canvas* board = new p10canvas(nullptr, QPoint(2,2), setting.resolution);
		QSize sz = board->size();
		setCentralWidget(board);
		centralWidget()->setFixedSize(sz);
		//centralWidget()->setMinimumSize(sz);
		//centralWidget()->setMaximumSize(sz);
		createMenubar();
		createToolbar();
		createStatusbar();
		for(int i(0); i<8; i++) {
			QObject::connect(
				mbtns[i],&ColorPicker::clicked, board, 
				[=](){board->pickColor(sfBtnColor[i]);}
			);
		}
		mbtns[1]->click(); // Default selected color
		setFixedSize(sizeHint());
	}
	~MainWindow(){}
};

void test() {
	qDebug() << "testmode";
	p10frame* frame = new p10frame("export.frame");
	frame->genTestFile();
	frame->save();
}

int main(int argc, char **argv) {
	if(argc == 2) {
		if(!strcmp(argv[1],"test")) {
			test();
			return 0;
		}
	}
	QApplication app (argc, argv);
	// Apply style file
	// QFile stylef("style/style.qss");
	// stylef.open(QFile::ReadOnly);
	// QString style(stylef.readAll());
	// app.setStyleSheet(style);
	
	MainWindow* window = new MainWindow;
	//window->resize(window_width + 10, window_height+10);
	window->setWindowTitle("P10 Frame Maker");
	//window->exe();
	window->show();
	p10frame frame();
	//std::fstream f("test.txt", std::fstream::out);
	//f << "test";
	//f.close();
	return app.exec();
}