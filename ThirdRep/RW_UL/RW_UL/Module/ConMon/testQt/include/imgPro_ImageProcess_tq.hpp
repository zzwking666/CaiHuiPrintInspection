#pragma once

#include <QThread>


class ImgPro_ImageProcess_tq
	: public QThread
{
	Q_OBJECT
public:
	ImgPro_ImageProcess_tq();
	~ImgPro_ImageProcess_tq();
public:
	void build_imgPro();
private:
public:
	void startTest();
	void run() override;
signals:
	void imgReady(QPixmap pixmap);
};
