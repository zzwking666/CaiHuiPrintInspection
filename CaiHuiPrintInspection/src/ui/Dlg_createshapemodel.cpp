#include "Dlg_createshapemodel.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QtGlobal>
#include <algorithm>
#include <halconcpp/HalconCpp.h>
#include "NumberKeyboard.h"

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
   if (_halconDisplay)
    {
        _halconDisplay->clearOverlayDrawer();
    }
   delete ui;
}

void Dlg_createshapemodel::build_ui()
{
    refresh_ui_from_data();

   _halconDisplay = std::make_unique<rw::rqw::HalconDisplay>(ui->label_imgDisplay);
    if (_halconDisplay)
    {
        _halconDisplay->initialize();
        _halconDisplay->setOverlayDrawer([this](HalconCpp::HTuple* windowHandle) {
            if (!windowHandle || !_hasHalconData)
            {
                return;
            }

            try
            {
                using namespace HalconCpp;

                SetLineWidth(*windowHandle, 2);
                SetDraw(*windowHandle, "margin");

                SetColor(*windowHandle, "green");
                for (const auto& region : _halconData.createRegions)
                {
                    if (region.IsInitialized())
                    {
                        DispObj(region, *windowHandle);
                    }
                }

                SetColor(*windowHandle, "yellow");
                for (const auto& region : _halconData.shieldRegions)
                {
                    if (region.IsInitialized())
                    {
                        DispObj(region, *windowHandle);
                    }
                }

                if (_modelContours.IsInitialized())
                {
                    SetColor(*windowHandle, "cyan");
                    SetLineWidth(*windowHandle, 2);
                    DispObj(_modelContours, *windowHandle);
                }
            }
            catch (...)
            {
            }
            });

        QTimer::singleShot(0, this, [this]() {
            if (_halconDisplay)
            {
                _halconDisplay->initialize();
                refresh_display_with_regions();
            }
        });
    }
}

void Dlg_createshapemodel::refresh_ui_from_data()
{
    auto& halconDatas = Modules::getInstance().configManagerModule.halconDatas;
    const int index = _templateIndex - 1;
    if (index < 0 || index >= halconDatas.size())
    {
        _halconData = HalconData();
        _hasHalconData = true;
        _drawHistory.clear();
        _modelContours.Clear();
    }
    else
    {
        _halconData = halconDatas[index];
        _hasHalconData = true;
    }

    ui->btn_baoguang->setText(QString::number(_halconData.baoguang));
    ui->btn_zengyi->setText(QString::number(_halconData.zengyi));

    ui->ckb_mean->setChecked(_halconData.isMeaning);
    ui->btn_mean->setText(QString::number(_halconData.meaning));

    if (_halconData.isContrast)
    {
        ui->rbtn_manual->setChecked(true);
    }
    else
    {
        ui->rbtn_auto->setChecked(true);
    }

    ui->btn_maxcontrast->setText(QString::number(_halconData.maxcontrast));
    ui->btn_mincontrast->setText(QString::number(_halconData.mincontrast));

    if (auto* ckbFindShapeModel = this->findChild<QCheckBox*>("ckb_findShapemodel"))
    {
        ckbFindShapeModel->setChecked(_halconData.ckb_findShapemodel);
    }
}

void Dlg_createshapemodel::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    refresh_ui_from_data();

    if (_halconDisplay)
    {
        _halconDisplay->initialize();
        refresh_display_with_regions();
    }
}

void Dlg_createshapemodel::build_connect()
{
  QObject::connect(ui->btn_exit, &QPushButton::clicked,
        this, &Dlg_createshapemodel::btn_exit_clicked);
   QObject::connect(ui->btn_createShapeModel, &QPushButton::clicked,
       this, &Dlg_createshapemodel::btn_createShapeModel_clicked);
   QObject::connect(ui->btn_paintRegion, &QPushButton::clicked,
        this, &Dlg_createshapemodel::btn_paintRegion_clicked);
   QObject::connect(ui->btn_readImage, &QPushButton::clicked,
       this, &Dlg_createshapemodel::btn_readImage_clicked);
   QObject::connect(ui->btn_shiledRegion, &QPushButton::clicked,
        this, &Dlg_createshapemodel::btn_shiledRegion_clicked);
    QObject::connect(ui->btn_clearRegion2, &QPushButton::clicked,
        this, &Dlg_createshapemodel::btn_clearRegion2_clicked);
    QObject::connect(ui->btn_clearRegion, &QPushButton::clicked,
        this, &Dlg_createshapemodel::btn_clearRegion_clicked);

    QObject::connect(ui->btn_mean, &QPushButton::clicked,
        this, &Dlg_createshapemodel::btn_mean_clicked);
    QObject::connect(ui->btn_maxcontrast, &QPushButton::clicked,
        this, &Dlg_createshapemodel::btn_maxcontrast_clicked);
    QObject::connect(ui->btn_mincontrast, &QPushButton::clicked,
        this, &Dlg_createshapemodel::btn_mincontrast_clicked);
    QObject::connect(ui->ckb_mean, &QCheckBox::toggled,
        this, &Dlg_createshapemodel::ckb_mean_toggled);
    QObject::connect(ui->rbtn_auto, &QRadioButton::toggled,
        this, &Dlg_createshapemodel::rbtn_auto_toggled);
    QObject::connect(ui->rbtn_manual, &QRadioButton::toggled,
        this, &Dlg_createshapemodel::rbtn_manual_toggled);

    if (auto* ckbFindShapeModel = this->findChild<QCheckBox*>("ckb_findShapemodel"))
    {
        QObject::connect(ckbFindShapeModel, &QCheckBox::toggled, this,
            [this](bool checked)
            {
                _halconData.ckb_findShapemodel = checked;
            });
    }
}

void Dlg_createshapemodel::btn_exit_clicked()
{
    auto reply = QMessageBox::question(this, tr("提示"), tr("是否保存当前参数？"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

    if (reply == QMessageBox::Cancel)
    {
        return;
    }

    if (reply == QMessageBox::Yes)
    {
        _halconData.baoguang = ui->btn_baoguang->text().toDouble();
        _halconData.zengyi = ui->btn_zengyi->text().toDouble();
        _halconData.isMeaning = ui->ckb_mean->isChecked();
        _halconData.meaning = ui->btn_mean->text().toDouble();
        _halconData.isContrast = ui->rbtn_manual->isChecked();
        _halconData.maxcontrast = ui->btn_maxcontrast->text().toDouble();
        _halconData.mincontrast = ui->btn_mincontrast->text().toDouble();
        if (auto* ckbFindShapeModel = this->findChild<QCheckBox*>("ckb_findShapemodel"))
        {
            _halconData.ckb_findShapemodel = ckbFindShapeModel->isChecked();
        }

        auto& halconDatas = Modules::getInstance().configManagerModule.halconDatas;
        const int index = _templateIndex - 1;
        if (index >= 0)
        {
            if (halconDatas.size() <= index)
            {
                halconDatas.resize(index + 1);
            }
            halconDatas[index] = _halconData;
            _hasHalconData = true;
        }
    }

    accept();
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

    HalconCpp::ReadImage(&_halconData.processImage, imagePath.toStdString().c_str());
    _hasHalconData = true;
    _drawHistory.clear();
    _modelContours.Clear();
    refresh_display_with_regions();
}
void Dlg_createshapemodel::btn_createShapeModel_clicked()
{
    if (!_hasHalconData || !_halconData.processImage.IsInitialized())
    {
        QMessageBox::warning(this, tr("提示"), tr("请先读取图片"));
        return;
    }

    if (_halconData.createRegions.isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("请先绘制创建区域"));
        return;
    }

    try
    {
        using namespace HalconCpp;

        auto buildUnionFromRegions = [](const QVector<HObject>& regions, HObject* outUnion)
            {
                HObject concatRegions;
                GenEmptyObj(&concatRegions);

                for (const auto& region : regions)
                {
                    if (!region.IsInitialized())
                    {
                        continue;
                    }

                    HObject tmp;
                    ConcatObj(concatRegions, region, &tmp);
                    concatRegions = tmp;
                }

                Union1(concatRegions, outUnion);
            };

        HObject createUnion;
        buildUnionFromRegions(_halconData.createRegions, &createUnion);

        HObject modelRegion = createUnion;
        if (!_halconData.shieldRegions.isEmpty())
        {
            HObject shieldUnion;
            buildUnionFromRegions(_halconData.shieldRegions, &shieldUnion);

            HObject diffRegion;
            Difference(createUnion, shieldUnion, &diffRegion);
            modelRegion = diffRegion;
        }

        HTuple hvArea, hvRow, hvCol;
        AreaCenter(modelRegion, &hvArea, &hvRow, &hvCol);
        if (hvArea.TupleLength() <= 0 || hvArea.TupleSum().D() <= 0.0)
        {
            QMessageBox::warning(this, tr("提示"), tr("有效建模区域为空，请检查创建/屏蔽区域"));
            return;
        }

        HObject imageForModel = _halconData.processImage;
        if (_halconData.isMeaning)
        {
            int meanSize = static_cast<int>(std::round(_halconData.meaning));
            if (meanSize < 1)
            {
                meanSize = 1;
            }
            if (meanSize % 2 == 0)
            {
                ++meanSize;
            }

            HObject meanImage;
            MeanImage(imageForModel, &meanImage, meanSize, meanSize);
            imageForModel = meanImage;
        }

        HObject reducedImage;
        ReduceDomain(imageForModel, modelRegion, &reducedImage);
        imageForModel = reducedImage;

        if (_halconData.hv_ModelID.TupleLength() > 0)
        {
            try
            {
                ClearShapeModel(_halconData.hv_ModelID);
            }
            catch (...)
            {
            }
            _halconData.hv_ModelID = HTuple();
        }

        HTuple hvModelID;
        if (_halconData.isContrast)
        {
            CreateShapeModel(imageForModel,
                "auto",
                -3.1415926,
                6.2831852,
                "auto",
                "auto",
                "use_polarity",
                _halconData.maxcontrast,
                _halconData.mincontrast,
                &hvModelID);
        }
        else
        {
            CreateShapeModel(imageForModel,
                "auto",
                -3.1415926,
                6.2831852,
                "auto",
                "auto",
                "use_polarity",
                "auto",
                "auto",
                &hvModelID);
        }

        _halconData.hv_ModelID = hvModelID;

        HObject modelContours;
        GetShapeModelContours(&modelContours, hvModelID, 1);

        HTuple hvFindRow, hvFindCol, hvFindAngle, hvFindScore;
        FindShapeModel(imageForModel,
            hvModelID,
            -3.1415926,
            6.2831852,
            0.1,
            1,
            0.5,
            "least_squares",
            0,
            0.7,
            &hvFindRow,
            &hvFindCol,
            &hvFindAngle,
            &hvFindScore);

        _modelContours.Clear();
        if (hvFindRow.TupleLength() > 0)
        {
            HTuple hvHomMat2D;
            VectorAngleToRigid(0.0, 0.0, 0.0,
                hvFindRow[0].D(), hvFindCol[0].D(), hvFindAngle[0].D(),
                &hvHomMat2D);

            HObject foundContours;
            AffineTransContourXld(modelContours, &foundContours, hvHomMat2D);
            _modelContours = foundContours;

            auto& halconDatas = Modules::getInstance().configManagerModule.halconDatas;
            const int index = _templateIndex - 1;
            if (index >= 0)
            {
                if (halconDatas.size() <= index)
                {
                    halconDatas.resize(index + 1);
                }
                halconDatas[index] = _halconData;
            }

            QMessageBox::information(this, tr("提示"), tr("模板创建成功"));
            refresh_display_with_regions();
        }
        else
        {
            _halconData.hv_ModelID = HalconCpp::HTuple();
            try
            {
                HalconCpp::ClearShapeModel(hvModelID);
            }
            catch (...)
            {
            }
            QMessageBox::warning(this, tr("提示"), tr("创建模板失败: 未找到匹配轮廓"));
            return;
        }
    }
    catch (const HalconCpp::HException& e)
    {
        QMessageBox::warning(this, tr("提示"), tr("创建模板失败: %1").arg(QString::fromStdString(e.ErrorMessage().Text())));
    }
    catch (...)
    {
        QMessageBox::warning(this, tr("提示"), tr("创建模板失败"));
    }
}
void Dlg_createshapemodel::btn_paintRegion_clicked()
{
    drawRectangleAndStore(false);
}



void Dlg_createshapemodel::btn_shiledRegion_clicked()
{
   drawRectangleAndStore(true);
}

void Dlg_createshapemodel::btn_clearRegion_clicked()
{
   if (!_hasHalconData)
    {
        return;
    }

    _halconData.createRegions.clear();
    _halconData.shieldRegions.clear();
    _drawHistory.clear();
    _modelContours.Clear();
    refresh_display_with_regions();
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
    if (!checked)
    {
        return;
    }

    _halconData.isContrast = false;
    ui->rbtn_manual->setChecked(false);
}

void Dlg_createshapemodel::rbtn_manual_toggled(bool checked)
{
    if (!checked)
    {
        return;
    }

    _halconData.isContrast = true;
    ui->rbtn_auto->setChecked(false);
}

void Dlg_createshapemodel::btn_contrast_clicked()
{
}

void Dlg_createshapemodel::btn_mincontrast_clicked()
{
    NumberKeyboard numKeyBord;
    numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
    auto isAccept = numKeyBord.exec();
    if (isAccept == QDialog::Accepted)
    {
        auto value = numKeyBord.getValue();
        if (value.toDouble() < 0)
        {
            QMessageBox::warning(this, tr("提示"), tr("请输入大于等于0的数值"));
            return;
        }

        _halconData.mincontrast = value.toDouble();
        ui->btn_mincontrast->setText(value);
    }
}

void Dlg_createshapemodel::btn_maxcontrast_clicked()
{
    NumberKeyboard numKeyBord;
    numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
    auto isAccept = numKeyBord.exec();
    if (isAccept == QDialog::Accepted)
    {
        auto value = numKeyBord.getValue();
        if (value.toDouble() < 0)
        {
            QMessageBox::warning(this, tr("提示"), tr("请输入大于等于0的数值"));
            return;
        }

        _halconData.maxcontrast = value.toDouble();
        ui->btn_maxcontrast->setText(value);
    }
}

void Dlg_createshapemodel::btn_clearRegion2_clicked()
{
   if (!_hasHalconData || _drawHistory.isEmpty())
    {
        return;
    }

    const bool lastIsShield = _drawHistory.back();
    _drawHistory.pop_back();

    if (lastIsShield)
    {
        if (!_halconData.shieldRegions.isEmpty())
        {
            _halconData.shieldRegions.removeLast();
        }
    }
    else
    {
        if (!_halconData.createRegions.isEmpty())
        {
            _halconData.createRegions.removeLast();
        }
    }

    refresh_display_with_regions();
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
    NumberKeyboard numKeyBord;
    numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
    auto isAccept = numKeyBord.exec();
    if (isAccept == QDialog::Accepted)
    {
        auto value = numKeyBord.getValue();
        if (value.toDouble() < 0)
        {
            QMessageBox::warning(this, tr("提示"), tr("请输入大于等于0的数值"));
            return;
        }

        _halconData.meaning = value.toDouble();
        ui->btn_mean->setText(value);
    }
}

void Dlg_createshapemodel::ckb_mean_toggled(bool checked)
{
    _halconData.isMeaning = checked;
}

bool Dlg_createshapemodel::drawRectangleAndStore(bool isShieldRegion)
{
    if (!_hasHalconData)
    {
        QMessageBox::warning(this, tr("提示"), tr("请先读取图片"));
        return false;
    }

    if (!_halconDisplay || !_halconDisplay->isValid())
    {
        QMessageBox::warning(this, tr("提示"), tr("显示窗口未初始化"));
        return false;
    }

    if (!_halconData.processImage.IsInitialized())
    {
        QMessageBox::warning(this, tr("提示"), tr("当前没有可绘制的图片"));
        return false;
    }

    try
    {
        using namespace HalconCpp;

        _halconDisplay->setInteractionEnabled(false);

        if (auto* windowHandle = _halconDisplay->getWindowHandle())
        {
            SetColor(*windowHandle, isShieldRegion ? "yellow" : "green");
            SetLineWidth(*windowHandle, 2);
            SetDraw(*windowHandle, "margin");
        }

        refresh_display_with_regions();

        HTuple hvRow1, hvCol1, hvRow2, hvCol2;
        DrawRectangle1(*_halconDisplay->getWindowHandle(), &hvRow1, &hvCol1, &hvRow2, &hvCol2);

        HObject rectangle;
        GenRectangle1(&rectangle,
            hvRow1[0].D(), hvCol1[0].D(),
            hvRow2[0].D(), hvCol2[0].D());

        if (isShieldRegion)
        {
            _halconData.shieldRegions.push_back(rectangle);
        }
        else
        {
            _halconData.createRegions.push_back(rectangle);
        }

        _drawHistory.push_back(isShieldRegion);
        refresh_display_with_regions();
        _halconDisplay->setInteractionEnabled(true);
        return true;
    }
    catch (const HalconCpp::HException& e)
    {
        _halconDisplay->setInteractionEnabled(true);
        QMessageBox::warning(this, tr("提示"), tr("绘制区域失败: %1").arg(QString::fromStdString(e.ErrorMessage().Text())));
        return false;
    }
    catch (...)
    {
        _halconDisplay->setInteractionEnabled(true);
        QMessageBox::warning(this, tr("提示"), tr("绘制区域失败"));
        return false;
    }
}

void Dlg_createshapemodel::refresh_display_with_regions()
{
    if (!_halconDisplay || !_halconDisplay->isValid() || !_hasHalconData)
    {
        return;
    }

    if (!_halconData.processImage.IsInitialized())
    {
        return;
    }

    try
    {
        using namespace HalconCpp;

        auto* windowHandle = _halconDisplay->getWindowHandle();
        if (!windowHandle)
        {
            return;
        }

        // 不重置 SetPart，保持当前缩放/平移视图，仅重绘内容
        ClearWindow(*windowHandle);
        DispObj(_halconData.processImage, *windowHandle);

        SetLineWidth(*windowHandle, 2);
        SetDraw(*windowHandle, "margin");

        SetColor(*windowHandle, "green");
        for (const auto& region : _halconData.createRegions)
        {
            if (region.IsInitialized())
            {
                DispObj(region, *windowHandle);
            }
        }

        SetColor(*windowHandle, "yellow");
        for (const auto& region : _halconData.shieldRegions)
        {
            if (region.IsInitialized())
            {
                DispObj(region, *windowHandle);
            }
        }

        if (_modelContours.IsInitialized())
        {
            SetColor(*windowHandle, "cyan");
            SetLineWidth(*windowHandle, 2);
            DispObj(_modelContours, *windowHandle);
        }
    }
    catch (...)
    {
    }
}
