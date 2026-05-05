#include "DlgProductSet.h"
#include "ui_DlgProductSet.h"

#include <QMessageBox>
#include <QTabWidget>
#include <QtConcurrent/qtconcurrentrun.h>
#include "Modules.hpp"
#include "NumberKeyboard.h"

DlgProductSet::DlgProductSet(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::DlgProductSetClass())
{
	ui->setupUi(this);

	build_ui();

	build_connect();
}

DlgProductSet::~DlgProductSet()
{
	delete ui;
}

void DlgProductSet::build_ui()
{
	read_config();
}

void DlgProductSet::read_config()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;

	// 普通参数
	ui->btn_xiangjiguangdianpingbishijian->setText(QString::number(setConfig.xiangjiguangdianpingbishijian));
	ui->btn_shuchuxinhaoyanshi->setText(QString::number(setConfig.shuchuxinhaoyanshi));
	ui->btn_shuchuxinhaochixushijian->setText(QString::number(setConfig.shuchuxinhaochixushijian));

	// 相机参数
	ui->btn_shangxianwei1->setText(QString::number(setConfig.shangxianwei1));
	ui->btn_xiaxianwei1->setText(QString::number(setConfig.xiaxianwei1));
	ui->btn_zuoxianwei1->setText(QString::number(setConfig.zuoxianwei1));
	ui->btn_youxianwei1->setText(QString::number(setConfig.youxianwei1));
	ui->btn_baoguang1->setText(QString::number(setConfig.baoguang1));
	ui->btn_zengyi1->setText(QString::number(setConfig.zengyi1));
	ui->btn_xiangsudangliang1->setText(QString::number(setConfig.xiangsudangliang1));

	ui->btn_shangxianwei2->setText(QString::number(setConfig.shangxianwei2));
	ui->btn_xiaxianwei2->setText(QString::number(setConfig.xiaxianwei2));
	ui->btn_zuoxianwei2->setText(QString::number(setConfig.zuoxianwei2));
	ui->btn_youxianwei2->setText(QString::number(setConfig.youxianwei2));
	ui->btn_baoguang2->setText(QString::number(setConfig.baoguang2));
	ui->btn_zengyi2->setText(QString::number(setConfig.zengyi2));
	ui->btn_xiangsudangliang2->setText(QString::number(setConfig.xiangsudangliang2));

	ui->cbox_changeLanguage->setCurrentIndex(setConfig.changeLanguageIndex);
	changeLanguage(setConfig.changeLanguageIndex);

	ui->tabWidget->setCurrentIndex(0);
}

void DlgProductSet::build_connect()
{
	connect(ui->btn_close, &QPushButton::clicked, this, &DlgProductSet::btn_close_clicked);
	connect(ui->btn_xiangjiguangdianpingbishijian, &QPushButton::clicked, this, &DlgProductSet::btn_xiangjiguangdianpingbishijian_clicked);
	connect(ui->btn_shuchuxinhaoyanshi, &QPushButton::clicked, this, &DlgProductSet::btn_shuchuxinhaoyanshi_clicked);
	connect(ui->btn_shuchuxinhaochixushijian, &QPushButton::clicked, this, &DlgProductSet::btn_shuchuxinhaochixushijian_clicked);
	connect(ui->btn_testTrigger1_1, &QPushButton::clicked, this, &DlgProductSet::btn_testTrigger1_1_clicked);
	connect(ui->btn_testTrigger2_1, &QPushButton::clicked, this, &DlgProductSet::btn_testTrigger2_1_clicked);
	connect(ui->btn_testTrigger1_2, &QPushButton::clicked, this, &DlgProductSet::btn_testTrigger1_2_clicked);
	connect(ui->btn_testTrigger2_2, &QPushButton::clicked, this, &DlgProductSet::btn_testTrigger2_2_clicked);
	connect(ui->btn_shangxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_shangxianwei1_clicked);
	connect(ui->btn_xiaxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_xiaxianwei1_clicked);
	connect(ui->btn_zuoxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_zuoxianwei1_clicked);
	connect(ui->btn_youxianwei1, &QPushButton::clicked, this, &DlgProductSet::btn_youxianwei1_clicked);
	connect(ui->btn_baoguang1, &QPushButton::clicked, this, &DlgProductSet::btn_baoguang1_clicked);
	connect(ui->btn_zengyi1, &QPushButton::clicked, this, &DlgProductSet::btn_zengyi1_clicked);
	connect(ui->btn_xiangsudangliang1, &QPushButton::clicked, this, &DlgProductSet::btn_xiangsudangliang1_clicked);
	connect(ui->btn_shangxianwei2, &QPushButton::clicked, this, &DlgProductSet::btn_shangxianwei2_clicked);
	connect(ui->btn_xiaxianwei2, &QPushButton::clicked, this, &DlgProductSet::btn_xiaxianwei2_clicked);
	connect(ui->btn_zuoxianwei2, &QPushButton::clicked, this, &DlgProductSet::btn_zuoxianwei2_clicked);
	connect(ui->btn_youxianwei2, &QPushButton::clicked, this, &DlgProductSet::btn_youxianwei2_clicked);
	connect(ui->btn_baoguang2, &QPushButton::clicked, this, &DlgProductSet::btn_baoguang2_clicked);
	connect(ui->btn_zengyi2, &QPushButton::clicked, this, &DlgProductSet::btn_zengyi2_clicked);
	connect(ui->btn_xiangsudangliang2, &QPushButton::clicked, this, &DlgProductSet::btn_xiangsudangliang2_clicked);
	connect(ui->cbox_changeLanguage, &QComboBox::currentIndexChanged, this, &DlgProductSet::changeLanguage);
}

void DlgProductSet::changeLanguage(int index)
{
	// 中文
	if (0 == index)
	{
		ui->lb_xiangjiguangdianpingbishijian->setText("相机光电屏蔽时间");
		ui->lb_xiangsudangliang1->setText("像素当量");

		ui->lb_shangxianwei1->setText("上限位");
		ui->lb_xiaxianwei1->setText("下限位");
		ui->lb_baoguang1->setText("曝光");
		ui->lb_zengyi1->setText("增益");

		ui->lb_changeLanguage->setText("切换语言");

		ui->lb_xiangjiguangdianpingbishijianUnit->setText("ms");

		ui->lb_shangxianweiUnit1->setText("Pix");
		ui->lb_xiaxianweiUnit1->setText("Pix");
		ui->lb_xiangsudangliangUnit1->setText("mm/Pix");

		ui->lb_shangxianwei2->setText("上限位");
		ui->lb_xiaxianwei2->setText("下限位");
		ui->lb_baoguang2->setText("曝光");
		ui->lb_zengyi2->setText("增益");
		ui->lb_shangxianweiUnit2->setText("Pix");
		ui->lb_xiaxianweiUnit2->setText("Pix");
		ui->lb_xiangsudangliang2->setText("像素当量");
		ui->lb_xiangsudangliangUnit2->setText("mm/Pix");

		ui->btn_testTrigger1_1->setText("测试触发1");
		ui->btn_testTrigger2_1->setText("测试触发2");
		ui->btn_testTrigger1_2->setText("测试触发1");
		ui->btn_testTrigger2_2->setText("测试触发2");

		ui->tabWidget->setTabText(0, "一相机");
		ui->tabWidget->setTabText(1, "二相机");
		ui->tabWidget->setTabText(2, "分数界面内容可选显示");
	}
	// 英文
	else if (1 == index)
	{
		// 参数页（顶部）
		ui->lb_xiangjiguangdianpingbishijian->setText("Camera sensor mask time");
		ui->lb_xiangsudangliang1->setText("Pixel size");

		// 相机参数通用 label（tab 内 Camera 1）
		ui->lb_shangxianwei1->setText("Upper limit");
		ui->lb_xiaxianwei1->setText("Lower limit");
		ui->lb_baoguang1->setText("Exposure");
		ui->lb_zengyi1->setText("Gain");

		// 语言切换
		ui->lb_changeLanguage->setText("Language");

		ui->lb_xiangjiguangdianpingbishijianUnit->setText("ms");

		ui->lb_shangxianweiUnit1->setText("px");
		ui->lb_xiaxianweiUnit1->setText("px");
		ui->lb_xiangsudangliangUnit1->setText("mm/px");

		// Camera 2 tab 内那套 label（注意 ui 里是 _2/_3/_4 命名）
		ui->lb_shangxianwei2->setText("Upper limit");
		ui->lb_xiaxianwei2->setText("Lower limit");
		ui->lb_baoguang2->setText("Exposure");
		ui->lb_zengyi2->setText("Gain");
		ui->lb_shangxianweiUnit2->setText("px");
		ui->lb_xiaxianweiUnit2->setText("px");
		ui->lb_xiangsudangliang2->setText("Pixel size");
		ui->lb_xiangsudangliangUnit2->setText("mm/px");

		// 测试触发
		ui->btn_testTrigger1_1->setText("Test trigger 1");
		ui->btn_testTrigger2_1->setText("Test trigger 2");
		ui->btn_testTrigger1_2->setText("Test trigger 1");
		ui->btn_testTrigger2_2->setText("Test trigger 2");

		// Tab 标题
		ui->tabWidget->setTabText(0, "Camera 1");
		ui->tabWidget->setTabText(1, "Camera 2");
		ui->tabWidget->setTabText(2, "Score page items");
	}

	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	setConfig.changeLanguageIndex = index;
	emit emit_changeLanguage(index);
}

void DlgProductSet::btn_close_clicked()
{
	emit paramsChanged();
	this->close();
}

void DlgProductSet::btn_xiangjiguangdianpingbishijian_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_xiangjiguangdianpingbishijian->setText(value);
		setConfig.xiangjiguangdianpingbishijian = value.toDouble();
	}
}

void DlgProductSet::btn_shuchuxinhaoyanshi_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shuchuxinhaoyanshi->setText(value);
		setConfig.shuchuxinhaoyanshi = value.toDouble();
	}
}

void DlgProductSet::btn_shuchuxinhaochixushijian_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shuchuxinhaochixushijian->setText(value);
		setConfig.shuchuxinhaochixushijian = value.toDouble();
	}
}

void DlgProductSet::btn_testTrigger1_1_clicked()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera = Modules::getInstance().cameraModule.camera1;
	// 剔废动作
	rw::rqw::OutTriggerConfig outTriggerConfig;
	outTriggerConfig.lineSelector = 1;
	outTriggerConfig.lineMode = 8;
	outTriggerConfig.lineSource = 5;
	outTriggerConfig.durationValue = 400 * 1000;
	outTriggerConfig.strobeEnable = true;
	if (camera)
	{
		camera->setOutTriggerConfig(outTriggerConfig);
		camera->outTrigger();
	}
}

void DlgProductSet::btn_testTrigger2_1_clicked()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera = Modules::getInstance().cameraModule.camera1;
	// 剔废动作
	rw::rqw::OutTriggerConfig outTriggerConfig;
	outTriggerConfig.lineSelector = 2;
	outTriggerConfig.lineMode = 8;
	outTriggerConfig.lineSource = 5;
	outTriggerConfig.durationValue = 400 * 1000;
	outTriggerConfig.strobeEnable = true;
	if (camera)
	{
		camera->setOutTriggerConfig(outTriggerConfig);
		camera->outTrigger();
	}
}

void DlgProductSet::btn_testTrigger1_2_clicked()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera = Modules::getInstance().cameraModule.camera2;
	// 剔废动作
	rw::rqw::OutTriggerConfig outTriggerConfig;
	outTriggerConfig.lineSelector = 1;
	outTriggerConfig.lineMode = 8;
	outTriggerConfig.lineSource = 5;
	outTriggerConfig.durationValue = 400 * 1000;
	outTriggerConfig.strobeEnable = true;
	if (camera)
	{
		camera->setOutTriggerConfig(outTriggerConfig);
		camera->outTrigger();
	}
}

void DlgProductSet::btn_testTrigger2_2_clicked()
{
	auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
	auto& camera = Modules::getInstance().cameraModule.camera2;
	// 剔废动作
	rw::rqw::OutTriggerConfig outTriggerConfig;
	outTriggerConfig.lineSelector = 2;
	outTriggerConfig.lineMode = 8;
	outTriggerConfig.lineSource = 5;
	outTriggerConfig.durationValue = 400 * 1000;
	outTriggerConfig.strobeEnable = true;
	if (camera)
	{
		camera->setOutTriggerConfig(outTriggerConfig);
		camera->outTrigger();
	}
}

void DlgProductSet::btn_shangxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shangxianwei1->setText(value);
		setConfig.shangxianwei1 = value.toDouble();
	}
}

void DlgProductSet::btn_xiaxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_xiaxianwei1->setText(value);
		setConfig.xiaxianwei1 = value.toDouble();
	}
}

void DlgProductSet::btn_zuoxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_zuoxianwei1->setText(value);
		setConfig.zuoxianwei1 = value.toDouble();
	}
}

void DlgProductSet::btn_youxianwei1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_youxianwei1->setText(value);
		setConfig.youxianwei1 = value.toDouble();
	}
}

void DlgProductSet::btn_baoguang1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		auto& camera = Modules::getInstance().cameraModule.camera1;
		if (camera)
		{
			camera->setExposureTime(value.toInt());
		}
		ui->btn_baoguang1->setText(value);
		setConfig.baoguang1 = value.toDouble();
	}
}

void DlgProductSet::btn_zengyi1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		auto& camera = Modules::getInstance().cameraModule.camera1;
		if (camera)
		{
			camera->setGain(value.toInt());
		}
		ui->btn_zengyi1->setText(value);
		setConfig.zengyi1 = value.toDouble();
	}
}

void DlgProductSet::btn_xiangsudangliang1_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() <= 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_xiangsudangliang1->setText(value);
		setConfig.xiangsudangliang1 = value.toDouble();
	}
}

void DlgProductSet::btn_shangxianwei2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_shangxianwei2->setText(value);
		setConfig.shangxianwei2 = value.toDouble();
	}
}

void DlgProductSet::btn_xiaxianwei2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_xiaxianwei2->setText(value);
		setConfig.xiaxianwei2 = value.toDouble();
	}
}

void DlgProductSet::btn_zuoxianwei2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_zuoxianwei2->setText(value);
		setConfig.zuoxianwei2 = value.toDouble();
	}
}

void DlgProductSet::btn_youxianwei2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_youxianwei2->setText(value);
		setConfig.youxianwei2 = value.toDouble();
	}
}

void DlgProductSet::btn_baoguang2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		auto& camera = Modules::getInstance().cameraModule.camera2;
		if (camera)
		{
			camera->setExposureTime(value.toInt());
		}
		ui->btn_baoguang2->setText(value);
		setConfig.baoguang2 = value.toDouble();
	}
}

void DlgProductSet::btn_zengyi2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() < 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于等于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		auto& camera = Modules::getInstance().cameraModule.camera2;
		if (camera)
		{
			camera->setGain(value.toInt());
		}
		ui->btn_zengyi2->setText(value);
		setConfig.zengyi2 = value.toDouble();
	}
}

void DlgProductSet::btn_xiangsudangliang2_clicked()
{
	NumberKeyboard numKeyBord;
	numKeyBord.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyBord.exec();
	if (isAccept == QDialog::Accepted)
	{
		auto value = numKeyBord.getValue();
		if (value.toDouble() <= 0)
		{
			QMessageBox::warning(this, "提示", "请输入大于0的数值");
			return;
		}
		auto& setConfig = Modules::getInstance().configManagerModule.setConfig;
		ui->btn_xiangsudangliang2->setText(value);
		setConfig.xiangsudangliang2 = value.toDouble();
	}
}