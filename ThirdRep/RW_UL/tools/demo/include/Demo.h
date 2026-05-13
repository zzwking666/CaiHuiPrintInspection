#pragma once

#include <QMainWindow>
#include"rqw_HalconWidget.hpp"
#include <halconcpp/HalconCpp.h>

QT_BEGIN_NAMESPACE
namespace Ui { class DemoClass; };
QT_END_NAMESPACE

class Demo : public QMainWindow
{
	Q_OBJECT
public:
	rw::rqw::HalconWidget* halconWidget;
public:
	Demo(QWidget* parent = nullptr);
	~Demo() override;
public:
	void build_ui();
	void build_connect();
public:
	void ini();
	HalconCpp::HImage image;

protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	Ui::DemoClass* ui;
public slots:
	void pushButton_clicked();

protected:
	void closeEvent(QCloseEvent* event) override;
};
