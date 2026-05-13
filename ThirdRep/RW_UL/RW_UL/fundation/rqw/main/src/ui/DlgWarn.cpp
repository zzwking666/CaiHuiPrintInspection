#include "DlgWarn.h"

#include "ui_DlgWarn.h"

DlgWarn::DlgWarn(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::DlgWarnClass())
{
	ui->setupUi(this);
	build_ui();
	build_connect();
}

DlgWarn::~DlgWarn()
{
	delete ui;
}

void DlgWarn::build_ui()
{
	this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
}

void DlgWarn::build_connect()
{
	connect(ui->pbtn_accept, &QPushButton::clicked,
		this, &DlgWarn::pbtn_accept_clicked);
	connect(ui->pbtn_ignore, &QPushButton::clicked,
		this, &DlgWarn::pbtn_ignore_clicked);
}

void DlgWarn::setTitle(const QString& Tile)
{
	ui->label_title->setText(Tile);
}

void DlgWarn::setText(const QString& text)
{
	ui->label_info->setText(text);
}

void DlgWarn::setTime(const QString& time)
{
	ui->label_time->setText(time);
}

void DlgWarn::pbtn_ignore_clicked()
{
	this->hide();
	emit isProcess();
}

void DlgWarn::pbtn_accept_clicked()
{
	this->hide();
	emit isProcess();
}