#pragma once

#include "ui_Dlg_createshapemodel.h"
#include <memory>
#include <QSize>
#include <QShowEvent>
#include "HalconDisplay.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class Dlg_createshapemodelClass; };
QT_END_NAMESPACE

class Dlg_createshapemodel : public QDialog
{
	Q_OBJECT
public:
    Dlg_createshapemodel(int templateIndex, QWidget* parent = nullptr);
	~Dlg_createshapemodel();

public:
	void build_ui();
	void build_connect();
	void refresh_ui_from_data();

protected:
	void showEvent(QShowEvent* event) override;

private slots:
	void btn_exit_clicked();
	void btn_readImage_clicked();
	void btn_paintRegion_clicked();

	void btn_createShapeModel_clicked();
	void btn_shiledRegion_clicked();
	void btn_clearRegion_clicked();
	void btn_paintCenterPoint_clicked();

	void rad_findshapemodel_toggled(bool checked);
	void rad_findshapemodelXLD_toggled(bool checked);

	void btn_MinLength_clicked();
	void btn_min_clicked();
	void btn_max_clicked();
	void btn_createXLD_clicked();

	void rbtn_auto_toggled(bool checked);
	void rbtn_manual_toggled(bool checked);
	void btn_contrast_clicked();
	void btn_mincontrast_clicked();

	void btn_clearRegion2_clicked();
	void btn_zengyi_clicked();
	void btn_baoguang_clicked();
	void btn_angle_clicked();
	void btn_opening_clicked();
	void btn_mean_clicked();
public:
	Ui::Dlg_createshapemodelClass* ui;
   std::unique_ptr<rw::rqw::HalconDisplay> _halconDisplay;
	int _templateIndex{ 1 };
  QSize _initialSize{};
};