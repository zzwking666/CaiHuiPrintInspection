#pragma once

#include <QDialog>
#include "NumberKeyboard.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PicturesPainterSetAspectRatioClass; };
QT_END_NAMESPACE

class PicturesPainterSetAspectRatio : public QDialog
{
	Q_OBJECT

public:
	PicturesPainterSetAspectRatio(QWidget* parent = nullptr);
	~PicturesPainterSetAspectRatio();
public:
	double getImageWidth() const;
	double getImageHeight() const;

private:
	void build_connect();

private slots:
	void btn_imgWidth_clicked();
	void btn_imgHeight_clicked();
	void btn_ok_clicked();
	void btn_exit_clicked();

private:
	Ui::PicturesPainterSetAspectRatioClass* ui;
};
