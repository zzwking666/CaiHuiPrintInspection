#include "Dlg_createshapemodel.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QtGlobal>
#include <algorithm>

#include "Modules.hpp"

Dlg_createshapemodel::Dlg_createshapemodel(int templateIndex, QWidget* parent)
   : QDialog(parent)
    , ui(new Ui::Dlg_createshapemodelClass())
    , _templateIndex(templateIndex)
{
   ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
    setSizeGripEnabled(true);
    _initialSize = size();
    setMinimumSize(_initialSize);

    build_ui();
    build_connect();
}

Dlg_createshapemodel::~Dlg_createshapemodel()
{
   delete ui;
}

void Dlg_createshapemodel::build_ui()
{
    refresh_ui_from_data();

   _halconDisplay = std::make_unique<rw::rqw::HalconDisplay>(ui->label_imgDisplay);
    if (_halconDisplay)
    {
        _halconDisplay->initialize();
    }
}

void Dlg_createshapemodel::refresh_ui_from_data()
{
    auto& halconDatas = Modules::getInstance().configManagerModule.halconDatas;
    const int index = _templateIndex - 1;
    if (index < 0 || index >= halconDatas.size())
    {
        return;
    }

    auto& halconData = halconDatas[index];

    ui->btn_baoguang->setText(QString::number(halconData.baoguang));
    ui->btn_zengyi->setText(QString::number(halconData.zengyi));

    ui->ckb_mean->setChecked(halconData.isMeaning);
    ui->btn_mean->setText(QString::number(halconData.meaning));

    if (halconData.isContrast)
    {
        ui->rbtn_manual->setChecked(true);
    }
    else
    {
        ui->rbtn_auto->setChecked(true);
    }

    ui->btn_maxcontrast->setText(QString::number(halconData.maxcontrast));
    ui->btn_mincontrast->setText(QString::number(halconData.mincontrast));
}

void Dlg_createshapemodel::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    refresh_ui_from_data();
}

void Dlg_createshapemodel::build_connect()
{
   QObject::connect(ui->btn_paintRegion, &QPushButton::clicked,
        this, &Dlg_createshapemodel::btn_paintRegion_clicked);
   QObject::connect(ui->btn_readImage, &QPushButton::clicked,
       this, &Dlg_createshapemodel::btn_readImage_clicked);
}

void Dlg_createshapemodel::btn_exit_clicked()
{
}

void Dlg_createshapemodel::btn_readImage_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(
        this,
        tr("选择图片"),
        QDir::homePath(),
        tr("Images (*.bmp *.jpg *.jpeg *.png *.tif *.tiff)")
    );

    if (imagePath.isEmpty())
    {
        return;
    }

    if (!_halconDisplay || !_halconDisplay->isValid())
    {
        QMessageBox::warning(this, tr("提示"), tr("显示窗口未初始化"));
        return;
    }

    if (!_halconDisplay->displayImageFromFile(imagePath, true))
    {
        QMessageBox::warning(this, tr("提示"), tr("图片显示失败"));
        return;
    }

    auto& halconDatas = Modules::getInstance().configManagerModule.halconDatas;
    const int index = _templateIndex - 1;
    if (index >= 0 && index < halconDatas.size())
    {
        HalconCpp::ReadImage(&halconDatas[index].processImage, imagePath.toStdString().c_str());
    }
}

void Dlg_createshapemodel::btn_paintRegion_clicked()
{
    



}

void Dlg_createshapemodel::btn_createShapeModel_clicked()
{
}

void Dlg_createshapemodel::btn_shiledRegion_clicked()
{
}

void Dlg_createshapemodel::btn_clearRegion_clicked()
{
}

void Dlg_createshapemodel::btn_paintCenterPoint_clicked()
{
}

void Dlg_createshapemodel::rad_findshapemodel_toggled(bool checked)
{
}

void Dlg_createshapemodel::rad_findshapemodelXLD_toggled(bool checked)
{
}

void Dlg_createshapemodel::btn_MinLength_clicked()
{
}

void Dlg_createshapemodel::btn_min_clicked()
{
}

void Dlg_createshapemodel::btn_max_clicked()
{
}

void Dlg_createshapemodel::btn_createXLD_clicked()
{
}

void Dlg_createshapemodel::rbtn_auto_toggled(bool checked)
{
}

void Dlg_createshapemodel::rbtn_manual_toggled(bool checked)
{
}

void Dlg_createshapemodel::btn_contrast_clicked()
{
}

void Dlg_createshapemodel::btn_mincontrast_clicked()
{
}

void Dlg_createshapemodel::btn_clearRegion2_clicked()
{
}

void Dlg_createshapemodel::btn_zengyi_clicked()
{
}

void Dlg_createshapemodel::btn_baoguang_clicked()
{
}

void Dlg_createshapemodel::btn_angle_clicked()
{
}

void Dlg_createshapemodel::btn_opening_clicked()
{
}

void Dlg_createshapemodel::btn_mean_clicked()
{
}
