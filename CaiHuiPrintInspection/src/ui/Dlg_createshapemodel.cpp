#include "Dlg_createshapemodel.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

Dlg_createshapemodel::Dlg_createshapemodel(QWidget* parent)
   : QDialog(parent)
    , ui(new Ui::Dlg_createshapemodelClass())
{
   ui->setupUi(this);

    build_ui();
    build_connect();
}

Dlg_createshapemodel::~Dlg_createshapemodel()
{
   delete ui;
}

void Dlg_createshapemodel::build_ui()
{
   _halconDisplay = std::make_unique<rw::rqw::HalconDisplay>(ui->label_imgDisplay);
    if (_halconDisplay)
    {
        _halconDisplay->initialize();
    }
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
