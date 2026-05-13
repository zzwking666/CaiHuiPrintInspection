#pragma once

#include <QMainWindow>
#include "ui_ModelConverter.h"

#include"Converter.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class ModelConverterClass; };
QT_END_NAMESPACE

class ModelConverter : public QMainWindow
{
	Q_OBJECT
private:
	Converter* _converter{ nullptr };
public:
	bool isKill{ false };
public:
	ModelConverter(QWidget* parent = nullptr);
	~ModelConverter();

private:
	void build_ui();
	void build_connect();

private:
	Ui::ModelConverterClass* ui;
private slots:
	void pbtn_openOnnxFilePath_clicked();
	void pbtn_outPutPath_clicked();
	void pbtn_exit_clicked();
	void pbtn_nextStep();
	void pbtn_tab1Exit_clicked();
	void pbtn_preStep_clicked();
	void pbtn_startConvert_clicked();
public:
	void on_appRunLog(QString log);
	void finish();
};
