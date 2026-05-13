#include "DlgShutdownWarn.h"

#include "ui_DlgShutdownWarn.h"

DlgShutdownWarn::DlgShutdownWarn(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::DlgShutdownWarnClass())
{
	ui->setupUi(this);

	build_ui();
	build_connect();
}

DlgShutdownWarn::~DlgShutdownWarn()
{
	delete ui;
}

void DlgShutdownWarn::build_ui()
{
	this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
}

void DlgShutdownWarn::build_connect()
{
}

void DlgShutdownWarn::setTimeValue(size_t s)
{
	QString text = "系统将在" + QString::number(s) + "s后关机！！！";
	ui->label_info->setText(text);
}