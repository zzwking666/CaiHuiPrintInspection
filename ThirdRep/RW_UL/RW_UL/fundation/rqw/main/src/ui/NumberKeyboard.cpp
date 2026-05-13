#include "NumberKeyboard.h"
#include "ui_NumberKeyboard.h"

#include"rqw_MessageBox.hpp"

bool NumberKeyboard::isEnableEmptyInput = false;

NumberKeyboard::NumberKeyboard(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::NumberKeyboardClass())
{
	ui->setupUi(this);
	build_ui();
	build_connect();
}

NumberKeyboard::~NumberKeyboard()
{
	delete ui;
}

QString NumberKeyboard::getValue() const
{
	if (value.isEmpty())
	{
		return "0";
	}
	return value;
}

void NumberKeyboard::build_ui()
{
	ui->lineEdit->setFocus();
	ui->lineEdit->setCursorPosition(value.length());
	ui->lineEdit->setFocusPolicy(Qt::NoFocus);
	ui->lineEdit->clearFocus();
	ui->lineEdit->setAttribute(Qt::WA_TransparentForMouseEvents, true); // 禁止鼠标事件
	ui->lineEdit->setFocusPolicy(Qt::NoFocus); // 禁止键盘焦点
}

void NumberKeyboard::build_connect()
{
	QObject::connect(ui->pbtn_num1, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_num1_clicked);
	QObject::connect(ui->pbtn_num2, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_num2_clicked);
	QObject::connect(ui->pbtn_num3, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_num3_clicked);
	QObject::connect(ui->pbtn_num4, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_num4_clicked);
	QObject::connect(ui->pbtn_num5, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_num5_clicked);
	QObject::connect(ui->pbtn_num6, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_num6_clicked);
	QObject::connect(ui->pbtn_num7, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_num7_clicked);
	QObject::connect(ui->pbtn_num8, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_num8_clicked);
	QObject::connect(ui->pbtn_num9, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_num9_clicked);
	QObject::connect(ui->pbtn_num0, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_num0_clicked);
	QObject::connect(ui->pbtn_bar, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_bar_clicked);
	QObject::connect(ui->pbtn_point, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_point_clicked);
	QObject::connect(ui->pbtn_delete, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_delete_clicked);
	QObject::connect(ui->pbtn_cancel, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_cancel_clicked);
	QObject::connect(ui->pbtn_ok, &QPushButton::clicked,
		this, &NumberKeyboard::pbtn_ok_clicked);
}

void NumberKeyboard::showEvent(QShowEvent* showEvent)
{
	QDialog::showEvent(showEvent);
	value.clear();
	ui->lineEdit->clear();
}

NumberKeyboard::InputResult NumberKeyboard::inputDataOnQPushButton(QPushButton* button, QString& value, const InputDataConfig& cfg)
{
	if (!button)
	{
		return InputResult::Ignore;
	}


	NumberKeyboard numKeyboard;
	numKeyboard.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint);
	auto isAccept = numKeyboard.exec();

	if (isEnableEmptyInput)
	{
		value = numKeyboard.getValue();
		if (value == "empty")
		{
			return InputResult::Ignore;
		}
	}

	if (isAccept == QDialog::Accepted)
	{
		value = numKeyboard.getValue();

		if (cfg.isUsingMax && cfg.isUsingMin)
		{
			if (value.toDouble() < cfg.min || value.toDouble() > cfg.max)
			{
				return InputResult::Reject;
			}
		}

		if (cfg.isUsingMax && (!cfg.isUsingMin))
		{
			if (value.toDouble() > cfg.max)
			{
				return InputResult::Reject;
			}
		}

		if ((!cfg.isUsingMax) && cfg.isUsingMin)
		{
			if (value.toDouble() < cfg.min)
			{
				return InputResult::Reject;
			}
		}

		button->setText(value);
		return InputResult::Accept;
	}
	return InputResult::Ignore;
}

void NumberKeyboard::pbtn_num1_clicked()
{
	value.append("1");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_num2_clicked()
{
	value.append("2");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_num3_clicked()
{
	value.append("3");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_num4_clicked()
{
	value.append("4");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_num5_clicked()
{
	value.append("5");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_num6_clicked()
{
	value.append("6");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_num7_clicked()
{
	value.append("7");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_num8_clicked()
{
	value.append("8");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_num9_clicked()
{
	value.append("9");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_num0_clicked()
{
	value.append("0");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_bar_clicked()
{
	value.append("-");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_point_clicked()
{
	value.append(".");
	ui->lineEdit->setText(value);
}

void NumberKeyboard::pbtn_delete_clicked()
{
	if (value.length() > 0)
	{
		value.remove(value.length() - 1, 1);
		ui->lineEdit->setText(value);
	}
}

void NumberKeyboard::pbtn_cancel_clicked()
{
	this->reject();
}

void NumberKeyboard::pbtn_ok_clicked()
{
	if (value.isEmpty())
	{
		if (!isEnableEmptyInput)
		{
			rw::rqw::MessageBox::warning(this, "错误", "未输入任何数值");
			return;
		}

		value = "empty";
	}
	this->accept();
}