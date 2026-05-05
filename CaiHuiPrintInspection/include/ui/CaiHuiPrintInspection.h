#pragma once

#include <QMainWindow>
#include <memory>
#include <atomic>
#include <QCheckBox>

#include "rqw_LabelClickable.h"
#include "DlgCloseForm.h"
#include "oso_StorageContext.hpp"
#include "rqw_RunEnvCheck.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class CaiHuiPrintInspectionClass; };
QT_END_NAMESPACE

class CaiHuiPrintInspection : public QMainWindow
{
	Q_OBJECT

public:
	CaiHuiPrintInspection(QWidget* parent = nullptr);
	~CaiHuiPrintInspection();
#ifdef BUILD_WITHOUT_HARDWARE
public:
	QCheckBox* cBox_testPushImg{ nullptr };
public slots:
	void cBox_testPushImg_checked(bool checked);
#endif
public:
	void build_ui();
	void build_connect();
	void build_CaiHuiPrintInspectionData();
	void ini_clickableTitle();
	void build_DlgCloseForm();
public:
	void initializeComponents();
public:
	void build_camera();
public slots:
	void changeLanguage(int index);
public slots:
	void updateCameraLabelState(int cameraIndex, bool state);

	void onUpdateStatisticalInfoUI();

	void onCameraDisplay(size_t index, QPixmap image);

	void lb_title_clicked();
private slots:
	void pbtn_exit_clicked();
	void pbtn_set_clicked();
	void rbtn_debug_checked(bool checked);
	void rbtn_removeFunc_checked(bool checked);
	void pbtn_resetProduct_clicked();
	void ckb_saveImg_checked(bool checked);
private:
	rw::rqw::ClickableLabel* clickableTitle = nullptr;
	DlgCloseForm* _dlgCloseForm = nullptr;
private:
	Ui::CaiHuiPrintInspectionClass* ui;
	int minimizeCount{ 3 };
};