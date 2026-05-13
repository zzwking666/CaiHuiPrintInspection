#include "PicturesPainter_SetAspectRatio.h"

#include <QMessageBox>

#include "ui_PicturesPainter_SetAspectRatio.h"

PicturesPainterSetAspectRatio::PicturesPainterSetAspectRatio(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::PicturesPainterSetAspectRatioClass())
{
	ui->setupUi(this);
	build_connect();
}

PicturesPainterSetAspectRatio::~PicturesPainterSetAspectRatio()
{
	delete ui;
}

double PicturesPainterSetAspectRatio::getImageWidth() const
{
	return ui->btn_imgWidth->text().toDouble();
}

double PicturesPainterSetAspectRatio::getImageHeight() const
{
	return ui->btn_imgHeight->text().toDouble();
}

void PicturesPainterSetAspectRatio::build_connect()
{
	QObject::connect(ui->btn_imgWidth, &QPushButton::clicked,
		this, &PicturesPainterSetAspectRatio::btn_imgWidth_clicked);
	QObject::connect(ui->btn_imgHeight, &QPushButton::clicked,
		this, &PicturesPainterSetAspectRatio::btn_imgHeight_clicked);
	QObject::connect(ui->btn_ok, &QPushButton::clicked,
		this, &PicturesPainterSetAspectRatio::btn_ok_clicked);
	QObject::connect(ui->btn_exit, &QPushButton::clicked,
		this, &PicturesPainterSetAspectRatio::btn_exit_clicked);
}

void PicturesPainterSetAspectRatio::btn_imgWidth_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		ui->btn_imgWidth->setText(value);
	}
}

void PicturesPainterSetAspectRatio::btn_imgHeight_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		ui->btn_imgHeight->setText(value);
	}
}

void PicturesPainterSetAspectRatio::btn_ok_clicked()
{
	double img_Width = ui->btn_imgWidth->text().toDouble();
	double img_Height = ui->btn_imgHeight->text().toDouble();
	if (img_Width <= 0 || img_Height <= 0)
	{
		QMessageBox::warning(this, "提示", "设定的图像宽度与高度必须大于0!");
		return;
	}

	this->accept();
}

void PicturesPainterSetAspectRatio::btn_exit_clicked()
{
	this->reject();
}