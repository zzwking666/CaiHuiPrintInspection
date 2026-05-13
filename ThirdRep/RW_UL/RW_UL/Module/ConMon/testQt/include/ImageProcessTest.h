#pragma once

#include <QMainWindow>

#include "imgPro_ImageProcess_tq.hpp"
#include"rqw_ImageSaveEngine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ImageProcessTestClass; };
QT_END_NAMESPACE

class ConditionMonitorTest : public QMainWindow
{
	Q_OBJECT

public:
	ConditionMonitorTest(QWidget *parent = nullptr);
	~ConditionMonitorTest();

public:
	void ini();
private:
	ImgPro_ImageProcess_tq* _imgPro;
	rw::rqw::ImageSaveEngine* _imageSaveEngine;
private:
	Ui::ImageProcessTestClass *ui;
public slots:
	void displayImage(QPixmap pixmap);
	void pbtn_startTest_clicked();
};
