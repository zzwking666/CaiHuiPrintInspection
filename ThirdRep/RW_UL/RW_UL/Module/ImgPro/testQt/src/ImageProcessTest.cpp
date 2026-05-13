#include "ImageProcessTest.h"

#include "ui_ImageProcessTest.h"

ImageProcessTest::ImageProcessTest(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::ImageProcessTestClass())
{
	ui->setupUi(this);

	ini();
}

ImageProcessTest::~ImageProcessTest()
{
	_imgPro->quit();
	delete _imgPro;
	delete ui;
}

void ImageProcessTest::ini()
{
	_imgPro = new ImgPro_ImageProcess_tq();
	QObject::connect(_imgPro, &ImgPro_ImageProcess_tq::imgReady,
		this, &ImageProcessTest::displayImage);
	QObject::connect(ui->pbtn_startTest, &QPushButton::clicked,
		this, &ImageProcessTest::pbtn_startTest_clicked,Qt::QueuedConnection);
	_imageSaveEngine = new rw::rqw::ImageSaveEngine(this, 4);
	_imageSaveEngine->setRootPath(R"(C:\Users\rw\Desktop\temp\tempImgs)");
	_imageSaveEngine->setSavePolicy(rw::rqw::ImageSaveEnginePolicy::Normal);
	_imageSaveEngine->setSaveImgFormat(rw::rqw::ImageSaveFormat::PNG);
	_imageSaveEngine->setSaveImgQuality(100);
	_imageSaveEngine->startEngine();
}

void ImageProcessTest::displayImage(QPixmap pixmap)
{
	if (pixmap.isNull()) {
		return;
	}
	_imageSaveEngine->pushImage(pixmap.toImage());
	ui->label_imgDis->setPixmap(pixmap.scaled(ui->label_imgDis->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ImageProcessTest::pbtn_startTest_clicked()
{
	if (_imgPro)
	{
		_imgPro->startTest();
	}
}
