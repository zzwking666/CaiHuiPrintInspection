#include "Demo.h"

#include "ui_Demo.h"
#include<Algorithm>
#include <qevent.h>

#include"opencv2/opencv.hpp"

Demo::Demo(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::DemoClass())
{
	ui->setupUi(this);
	build_ui();
	build_connect();
}

Demo::~Demo()
{
	delete ui;
}

void Demo::build_ui()
{
	halconWidget = new rw::rqw::HalconWidget(this);
	ui->verticalLayout->replaceWidget(ui->label, halconWidget);
	delete ui->label;
}

void Demo::build_connect()
{
	QObject::connect(ui->pushButton, &QPushButton::clicked,
		this, &Demo::pushButton_clicked);
}

void Demo::ini()
{
	/*HalconCpp::ReadImage(&image, "C:/Users/rw/Desktop/temp/4be85a13-4196-4ae1-bb3c-ccea8d1d27fa.png");
	Rgb1ToGray(image, &image);*/

	//QImage image("C:/Users/rw/Desktop/temp/4be85a13-4196-4ae1-bb3c-ccea8d1d27fa.png");

	auto image = cv::imread(R"(C:\Users\rw\Desktop\temp\f2e008f9-1ccd-42ea-94b0-cb25f39b9a47.png)");
	rw::rqw::HalconWidgetObject object(image);
	object.isShow = true;
	object.id = 0;
	auto a = object.has_value();
	halconWidget->appendHObject(object);
	auto id = halconWidget->getVailidAppendId();
	rw::rqw::PainterConfig config;
	config.color = rw::rqw::RQWColor::Green;
	halconWidget->appendHorizontalLine(halconWidget->height() / 2, config);
	halconWidget->appendVerticalLine(halconWidget->width() / 2, config);

	auto id2 = halconWidget->getVailidAppendId();
}

void Demo::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);
}

void Demo::pushButton_clicked()
{
	rw::rqw::PainterConfig config;
	auto rec = halconWidget->drawRect(config);
	auto id = halconWidget->createShapeXLDModel({ rec });
	rw::rqw::HalconShapeXLDFindConfig config1;
	rw::rqw::PainterConfig config2;
	config2.color = rw::rqw::RQWColor::Red;
	halconWidget->findShapeModel(id, config1, config2);
}

void Demo::closeEvent(QCloseEvent* event)
{
	if (halconWidget->isDrawing())
	{
		event->accept();
		return;
	}
}