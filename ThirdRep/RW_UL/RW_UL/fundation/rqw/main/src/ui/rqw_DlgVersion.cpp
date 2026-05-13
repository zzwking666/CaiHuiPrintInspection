#include "ui_rqw_DlgVersion.h"

#include "rqw_DlgVersion.h"
#include <QDir>

DlgVersion::DlgVersion(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::DlgVersionClass())
{
	ui->setupUi(this);
	ui->textEdit_VersionShow->setReadOnly(true); // 设置为只读
	build_connect();
}

DlgVersion::~DlgVersion()
{
	delete ui;
}

void DlgVersion::loadVersionPath(QString path)
{
	versionPath = path;
}

void DlgVersion::build_connect()
{
	QObject::connect(ui->pbtn_exit, &QPushButton::clicked,
		this, &DlgVersion::pbtn_exit_clicked);
	QObject::connect(ui->pbtn_bigger, &QPushButton::clicked,
		this, &DlgVersion::pbtn_bigger_clicked);
	QObject::connect(ui->pbtn_smaller, &QPushButton::clicked,
		this, &DlgVersion::pbtn_smaller_clicked);
}

void DlgVersion::showEvent(QShowEvent* event)
{
	QFile file(versionPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		ui->textEdit_VersionShow->setPlainText("无法打开版本文件: " + versionPath);
		return;
	}

	QTextStream in(&file);
	QString content = in.readAll();
	file.close();

	ui->textEdit_VersionShow->setPlainText(content);
}

void DlgVersion::pbtn_exit_clicked()
{
	this->close();
}

void DlgVersion::pbtn_bigger_clicked()
{
	ui->textEdit_VersionShow->zoomIn(1); // 放大文本
}

void DlgVersion::pbtn_smaller_clicked()
{
	ui->textEdit_VersionShow->zoomOut(1); // 缩小文本
}