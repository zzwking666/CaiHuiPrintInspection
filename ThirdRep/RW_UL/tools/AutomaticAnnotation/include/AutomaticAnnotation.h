#pragma once

#include <QMainWindow>

#include"PicturesViewer.h"
#include"AutomaticAnnotationThread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AutomaticAnnotationClass; };
QT_END_NAMESPACE

class AutomaticAnnotation : public QMainWindow
{
	Q_OBJECT

public:
	AutomaticAnnotation(QWidget* parent = nullptr);
	~AutomaticAnnotation();
private:
	PicturesViewer* viewer;
	QVector<AutomaticAnnotationThread*> threads;
public:
	int size{ 0 };
	int complete{ 0 };
	QString imageOutPut{};
	QString labelOutPut{};
public:
	void build_ui();
	void build_connect();
private:
	Ui::AutomaticAnnotationClass* ui;
private:
	void iniThread();

private slots:
	void pbtn_setImageInput_clicked();
	void pbtn_setAllOutput_clicked();
	void pbtn_setModelPath_clicked();
	void pbtn_setWorkers_clicked();
	void pbtn_setConfThreshold_clicked();
	void pbtn_nmsThreshold_clicked();
	void pbtn_exit_clicked();
	void pbtn_LookImage_clicked();
	void pbtn_next_clicked();
private slots:
	void on_pbtn_preStep_clicked();
	void on_pbtn_startAnnotation_clicked();
	void on_pbtn_tab2_exit_clicked();
public slots:
	void displayImage(QString imagePath, QPixmap pixmap);
public:
	rw::ModelType getModelType();
	rw::ModelEngineDeployType getDeployType();
};
