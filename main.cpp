/**
 * \file    main.cpp
 * \brief   A tool for creating frame file using led p10 module
 * \author  kienvo (kienlab.com)
 * \version 0.1
 * \date    Aug-12-2021
 * \copyright Copyright (c) 2021 by kienvo
 * \todo Replace all assert with exception handler
 */


#include <iostream>

#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QBoxLayout>
#include <QFrame>
#include <QMainWindow>

#include <qsfml/qsfml.h>
#include <assert.h>
#include "p10canvas/p10canvas.hpp"

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
	void paintEvent(QPaintEvent*) {
		QPainter painter(this);
		if(isChecked())drawSelectedBackground(painter);
		drawButton(painter);
	}
	void showEvent(QShowEvent*) {
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
	p10canvas* displayBoard;
	std::vector<p10canvas*> boards;
	QDockWidget* framelist;
	bool isEditing;

	void errorMessage() {
		
	}

	bool applySegmentToBoards(const p10frame* frame) {
		boards.resize(frame->getFileHeader().nSegments);
		for(int i(0); i<frame->getFileHeader().nSegments; i++) {
			std::vector<uint8_t> data;
			data.resize(frame->getFileHeader().displaySize.nPixels());

			const p10frame::FrameSegment* seg = frame->getSegmentNData(i, data);
			if(!seg) { // Check null
				statusBar()->showMessage(tr("Abort apply segments to boards."));
				QMessageBox::critical(this, tr("Error"),
					tr("Frame arguments is wrong.\n Abort apply segments to boards."),
					QMessageBox::Cancel
				);
				statusBar()->showMessage(tr("Abort apply segments to boards."),5000);
				return false;
			}

			boards[i] = new p10canvas(nullptr,  QPoint(2,2), {seg->frameSize.nCols, seg->frameSize.nRows});
			assert(boards[i]); // FIXME: assert test
			boards[i]->importSegment(*seg, data.data());
		}
		return true;
	}

	void updateSegment(QListWidgetItem* item) {
		qDebug() << item;
		// item->setBackgroundColor(Qt::red);
		auto* list = framelist->widget()->findChild<QListWidget*>();
		
		updateSegment(list->row(item));
	}
	
	void updateSegment(int segmentIndex) {
		if(displayBoard) {
			//centralWidget()->layout()->replaceWidget(displayBoard, boards[segmentIndex]);
			centralWidget()->layout()->removeWidget(displayBoard);
			displayBoard->setParent(nullptr);
		}
		displayBoard = boards[segmentIndex];
		centralWidget()->layout()->addWidget(displayBoard);
		centralWidget()->adjustSize();
		createConnects();  // reconnects
		mbtns[1]->click(); // Default selected color
	}

	void createToolbar() {
		QToolBar* colorpicker = addToolBar(tr("abc"));
        //colorpicker->addAction(newAct);
		for(int i(0); i<8; i++) {
			mbtns[i] = new ColorPicker(nullptr,qBtnColor[i]);
			mbtns[i]->setFixedSize(20,20);
			colorpicker->addWidget(mbtns[i]);
		}
	}
	
	void loadFromFile() {  // TODO: maintain this
		statusBar()->showMessage("Loading file ...");
		if(isEditing) {
			auto list = framelist->widget()->findChild<QListWidget*>();
			auto btn = framelist->widget()->findChild<QPushButton*>();
			framelist->widget()->layout()->removeWidget(list);
			framelist->widget()->layout()->removeWidget(btn);
			delete list;
			framelist->widget()->layout()->addWidget(new QListWidget);
			framelist->widget()->layout()->addWidget(btn);
		} 
		QString filename = QFileDialog::getOpenFileName(this, 
			tr("Open frame file"), "",  
			tr("Frame file (*.frame);;All Files (*)")
		);
		if(filename.isEmpty()) {
			statusBar()->showMessage("Unable to open file");
			return;
		}

		auto frame = new p10frame(filename.toLocal8Bit().data(), 
			std::fstream::binary
			| std::fstream::in
		);
		if(!frame->isValid()) {			
			statusBar()->showMessage(QString::fromStdString(frame->getErrorString()));
			QMessageBox::critical(this, tr("Error"),
				QString::fromStdString(frame->getErrorString()),
				QMessageBox::Cancel
			);
			statusBar()->clearMessage();
			return;
		}

		/* Display frame list */
		auto list = framelist->widget()->findChild<QListWidget*>();
		for(int i(0); i<frame->getFileHeader().nSegments; i++) {
			list->addItem(QString("Frame[%1]").arg(i));
			connect(list, &QListWidget::currentItemChanged, 
				[&](QListWidgetItem *item){
					updateSegment(item);
				}
			);
		}
		if(!applySegmentToBoards(frame)) {
			list->clear();
			return;
		}
		updateSegment(0);
		statusBar()->showMessage("");
		isEditing = true;
		statusBar()->clearMessage();		
	}

	void saveFileAs() {  // TODO: maintain this
		statusBar()->showMessage("Saving file ...");
		QString filename = QFileDialog::getSaveFileName(this,
			tr("Save Frame file"), "",
			tr("Frame file (*.frame);;All Files (*)")
		);
		if(filename.isEmpty()) {
			statusBar()->showMessage("Unable to open file");
			return;
		}

		auto* fr = new p10frame(filename.toLocal8Bit().data(), 
			std::fstream::binary
			| std::fstream::out
		);
		if(!fr->isValid()) {
			statusBar()->showMessage(QString::fromStdString(fr->getErrorString()));
			QMessageBox::critical(this, tr("Error"),
				QString::fromStdString(fr->getErrorString()),
				QMessageBox::Cancel
			);
			statusBar()->clearMessage();
			return;
		}
		p10frame::HeaderSection hd;
		std::vector<uint8_t> buf;
		hd.nSegments = boards.size();
		hd.displaySize = boards[0]->exportSegment(buf).frameSize;
		fr->setFileHeader(hd);
		for(int i(0); i<boards.size(); i++) {
			std::vector<uint8_t> tmp;
			const p10frame::FrameSegment s= boards[i]->exportSegment(tmp);
			fr->setSegmentData(i, s, tmp);
		}

		fr->save();
		delete fr;
	}

	void createMenubar() {
		auto* file = menuBar()->addMenu("File");
		file->addAction("Open file",[&](){
			loadFromFile();
		});
		file->addAction("Save file",[&](){
			saveFileAs();
		});
	}

	void createStatusbar() {
		statusBar()->showMessage(tr("Ready"), 5000);
		statusBar()->setStyleSheet("border-top: 1px outset grey;");
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
			setting.resolution.y =  p10rows[row.currentIndex()].second;
			setting.resolution.x = p10cols[col.currentIndex()].second;
			d.accept();
		});
		d.setWindowTitle("Please set resolution");
		d.setFixedSize(d.sizeHint());
		d.exec();
		std::cerr << "resolution " << setting.resolution.x << " " << setting.resolution.y << std::endl;
	}
	
	p10frame::DisplaySize popupSelectSizeDialog() {
		std::pair<QString, int> p10cols[] = {
			{"16", 16},
			{"32", 32},
			{"64", 64},
			{"96", 96},
			{"128", 128},
			{"160", 160},
			{"192", 192}
		};
		std::pair<QString, int> p10rows[] = {
			{"8", 8},
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
		p10frame::DisplaySize ret;
		QObject::connect(&done, &QPushButton::clicked, [&](){
			ret.nRows =  p10rows[row.currentIndex()].second;
			ret.nCols = p10cols[col.currentIndex()].second;
			d.accept();
		});
		d.setWindowTitle("Please set resolution");
		d.setFixedSize(d.sizeHint());
		d.exec();
		return ret;
	}

	void addNewSegment(p10frame::DisplaySize& dpsz) {		
		auto list = framelist->widget()->findChild<QListWidget*>();
		auto it = new QListWidgetItem(QString("Frame[%1]").arg(boards.size()));
		list->addItem(it);
		connect(list, &QListWidget::currentItemChanged, 
			[&](QListWidgetItem *item){
				updateSegment(item);
			}
		);
		boards.push_back( 
			new p10canvas(nullptr,  QPoint(2,2), {dpsz.nCols, dpsz.nRows})
		);

		//list->setItemSelected(it, true);
		list->currentItemChanged(it, it);
	}

	void createDock() {
		/* Init widget */
		framelist = new QDockWidget(tr("Dock Widget"), this);
		auto* multiWidget = new QWidget;
		auto* layout = new QVBoxLayout;		
		auto* tree = new QTreeWidget;
		auto* btn = new QPushButton("Add new frame");
		auto* list = new QListWidget;

		/* Bind layout */
		addDockWidget(Qt::RightDockWidgetArea, framelist);
		framelist->setWidget(multiWidget);
		multiWidget->setLayout(layout);
		//layout->addWidget(tree);
		layout->addWidget(list);
		layout->addWidget(btn);
		
		

		/* Set up */

		/* Actions */
		connect(btn, &QPushButton::clicked, [&](){
			p10frame::DisplaySize dpsz = popupSelectSizeDialog();
			addNewSegment(dpsz);
		});
	}

	void createDock1() {
		auto* dock = new QDockWidget(tr("Dock Widget"), this);
		auto* multiWidget = new QWidget;
		auto* layout = new QVBoxLayout;		
		auto* tree = new QTreeWidget;
		auto* btn = new QPushButton("Add new frame");
		

		tree->setColumnCount(1);
		//tree->header()->setStretchLastSection(false);
		//tree->header()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		//tree->header()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
		
		QList<QTreeWidgetItem *> items;
		items.append(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(QString("header"))));
		items.append(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(QString("segments"))));
		tree->insertTopLevelItems(0, items);

		items[0]->addChild(new QTreeWidgetItem(QStringList(QString("test"))));
		items[1]->addChild(new QTreeWidgetItem(QStringList(QString("Frame[0]"))));
		items[1]->addChild(new QTreeWidgetItem(QStringList(QString("Frame[1]"))));
		items[1]->addChild(new QTreeWidgetItem(QStringList(QString("Frame[2]"))));
		items[1]->addChild(new QTreeWidgetItem(QStringList(QString("Frame[3]"))));
		//items[0]->setSizeHint(0, {30,30});
		//tree->showMinimized();
		tree->resizeColumnToContents(0);
		tree->header()->setMinimumWidth(50);
		tree->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

		layout->addWidget(tree);
		tree->show();
		tree->resize(100,100);
		layout->addWidget(btn);
		multiWidget->setLayout(layout);
		
		//dock->setStyleSheet("border: 1px outset grey");
		dock->setAllowedAreas(Qt::LeftDockWidgetArea |Qt::RightDockWidgetArea);
		dock->setWidget(multiWidget);

		
		
		addDockWidget(Qt::RightDockWidgetArea, dock);
	}
	
	void createCentralWidget() {
		// displayBoard = new p10canvas(nullptr, QPoint(2,2), setting.resolution);
		// QSize sz = displayBoard->size();

		QWidget* central_widget = new QWidget;
		central_widget->setStyleSheet("border: 1px solid grey");
		QGridLayout* lay = new QGridLayout(central_widget);
		//lay->addWidget(displayBoard);
		setCentralWidget(central_widget);
		//board->setFixedSize(sz);
	}

	void createConnects() {
		for(int i(0); i<8; i++) {
			QObject::connect(
				mbtns[i],&ColorPicker::clicked, displayBoard, 
				[=](){displayBoard->pickColor(sfBtnColor[i]);}
			);
		}
	}

public:

	//using QMainWindow::QMainWindow;
	MainWindow(){
		displayBoard = nullptr;
		//popupConfigWindow();
		createCentralWidget();
		createMenubar();
		createToolbar();
		createStatusbar();
		//createConnects();
		createDock();
		mbtns[1]->click(); // Default selected color
	}
	~MainWindow(){}
};

void genSampleFile() {
	// qDebug() << "generate a sample file";
	p10frame* frame = new p10frame("export.frame", 		
		std::fstream::binary
		| std::fstream::out 
	);
	frame->genSampleFile();
	frame->save();
	std::cerr << "this is stderr" << endl;
	std::cout << "this is stdout" << endl;
}
bool importSampleFile() {
	qDebug() << "import sample file";
	p10frame* frame = new p10frame("export.frame",	
		std::fstream::binary
		| std::fstream::in
	);
}

int main(int argc, char **argv) {
	if(argc == 2) {
		if(!strcmp(argv[1],"gen")) {
			genSampleFile();
			return 0;
		}
		if(!strcmp(argv[1],"imp")) {
			importSampleFile();
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
	window->setWindowTitle("P10 Frame Maker");
	window->show();
	p10frame frame();

	return app.exec();
}