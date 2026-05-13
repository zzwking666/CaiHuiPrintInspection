#include "rqwu/rqwu_MessageBox.h"
#include <QIcon>
#include <QStyle>
#include <QVBoxLayout>
#include <QDialog>

namespace rw
{
	namespace rqwu
	{
		MessageBox::MessageBox(QWidget* parent)
			: QDialog(parent)
			, currentIcon(Icon::NoIcon)
			, result(0)
			, iconLabel(nullptr)
			, textLabel(nullptr)
			, buttonLayout(nullptr)
			, mainLayout(nullptr)
		{
			setupUI();
			setMinimumSize(500, 250);

			// 去掉窗体边框,设置为无边框窗口
			setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
			setAttribute(Qt::WA_TranslucentBackground, false);

			// 设置对话框整体样式
			setStyleSheet(
				"QDialog {"
				"    background-color: rgb(181, 181, 181);"
				"    border: 2px solid #e0e0e0;"
				"    border-radius: 15px;"
				"}"
			);
		}

		void MessageBox::setupUI()
		{
			mainLayout = new QVBoxLayout(this);
			mainLayout->setSpacing(20);
			mainLayout->setContentsMargins(30, 30, 30, 30);

			QHBoxLayout* contentLayout = new QHBoxLayout();
			contentLayout->setSpacing(25);

			iconLabel = new QLabel(this);
			iconLabel->setFixedSize(64, 64);
			iconLabel->setAlignment(Qt::AlignCenter);
			iconLabel->hide();
			contentLayout->addWidget(iconLabel);

			textLabel = new QLabel(this);
			textLabel->setWordWrap(true);
			textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
			textLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

			// 设置文本标签样式,与主界面保持一致
			textLabel->setStyleSheet(
				"QLabel {"
				"    font-size: 18px;"
				"    font-weight: bold;"
				"    color: rgb(85, 85, 85);"
				"    padding: 10px;"
				"    background-color: transparent;"
				"}"
			);

			contentLayout->addWidget(textLabel, 1);
			mainLayout->addLayout(contentLayout, 1);

			buttonLayout = new QHBoxLayout();
			buttonLayout->setSpacing(15);
			mainLayout->addLayout(buttonLayout);

			setLayout(mainLayout);
		}

		void MessageBox::setText(const QString& text)
		{
			textLabel->setText(text);
		}

		void MessageBox::setIcon(Icon icon)
		{
			currentIcon = icon;
			updateIcon();
		}

		void MessageBox::updateIcon()
		{
			QStyle::StandardPixmap pixmap;

			switch (currentIcon) {
			case Icon::Information:
				pixmap = QStyle::SP_MessageBoxInformation;
				break;
			case Icon::Warning:
				pixmap = QStyle::SP_MessageBoxWarning;
				break;
			case Icon::Critical:
				pixmap = QStyle::SP_MessageBoxCritical;
				break;
			case Icon::Question:
				pixmap = QStyle::SP_MessageBoxQuestion;
				break;
			default:
				iconLabel->hide();
				return;
			}

			QIcon icon = style()->standardIcon(pixmap);
			iconLabel->setPixmap(icon.pixmap(64, 64));
			iconLabel->show();
		}

		void MessageBox::setStandardButtons(int buttons)
		{
			QLayoutItem* item;
			while ((item = buttonLayout->takeAt(0)) != nullptr) {
				if (item->widget()) {
					delete item->widget();
				}
				delete item;
			}

			buttonLayout->addStretch();

			int yesButton = static_cast<int>(StandardButton::Yes);
			int noButton = static_cast<int>(StandardButton::No);
			int okButton = static_cast<int>(StandardButton::Ok);
			int cancelButton = static_cast<int>(StandardButton::Cancel);

			// 按钮样式,与主界面QPushButton样式一致
			QString buttonStyle =
				"QPushButton {"
				"    padding: 6px 14px;"
				"    border: 2px solid #CCC;"
				"    border-radius: 4px;"
				"    background-color: white;"
				"    color: #444;"
				"    font-size: 18px;"
				"    font-weight: bold;"
				"    min-width: 120px;"
				"    min-height: 45px;"
				"}"
				"QPushButton:hover {"
				"    border-color: #999;"
				"    background-color: #F5F5F5;"
				"}"
				"QPushButton:pressed {"
				"    border-color: #777;"
				"    background-color: #EEE;"
				"}"
				"QPushButton:default {"
				"    border: 2px solid #2196F3;"
				"    background-color: #E3F2FD;"
				"}";

			if (buttons & yesButton) {
				QPushButton* btnYes = new QPushButton("是(&Y)", this);
				btnYes->setStyleSheet(buttonStyle);
				connect(btnYes, &QPushButton::clicked, this, [this]() {
					result = static_cast<int>(StandardButton::Yes);
					accept();
					});
				buttonLayout->addWidget(btnYes);
			}

			if (buttons & noButton) {
				QPushButton* btnNo = new QPushButton("否(&N)", this);
				btnNo->setStyleSheet(buttonStyle);
				connect(btnNo, &QPushButton::clicked, this, [this]() {
					result = static_cast<int>(StandardButton::No);
					reject();
					});
				buttonLayout->addWidget(btnNo);
			}

			if (buttons & okButton) {
				QPushButton* btnOk = new QPushButton("确定(&O)", this);
				btnOk->setDefault(true);
				btnOk->setStyleSheet(buttonStyle);
				connect(btnOk, &QPushButton::clicked, this, [this]() {
					result = static_cast<int>(StandardButton::Ok);
					accept();
					});
				buttonLayout->addWidget(btnOk);
			}

			if (buttons & cancelButton) {
				QPushButton* btnCancel = new QPushButton("取消(&C)", this);
				btnCancel->setStyleSheet(buttonStyle);
				connect(btnCancel, &QPushButton::clicked, this, [this]() {
					result = static_cast<int>(StandardButton::Cancel);
					reject();
					});
				buttonLayout->addWidget(btnCancel);
			}

			buttonLayout->addStretch();
		}

		void MessageBox::onButtonClicked()
		{
			accept();
		}

		MessageBox::StandardButton MessageBox::information(QWidget* parent, const QString& title,
			const QString& text, int buttons)
		{
			MessageBox msgBox(parent);
			msgBox.setWindowTitle(title);
			msgBox.setText(text);
			msgBox.setIcon(Icon::Information);
			msgBox.setStandardButtons(buttons);
			msgBox.exec();
			return static_cast<StandardButton>(msgBox.result);
		}

		MessageBox::StandardButton MessageBox::warning(QWidget* parent, const QString& title,
			const QString& text, int buttons)
		{
			MessageBox msgBox(parent);
			msgBox.setWindowTitle(title);
			msgBox.setText(text);
			msgBox.setIcon(Icon::Warning);
			msgBox.setStandardButtons(buttons);
			msgBox.exec();
			return static_cast<StandardButton>(msgBox.result);
		}

		MessageBox::StandardButton MessageBox::critical(QWidget* parent, const QString& title,
			const QString& text, int buttons)
		{
			MessageBox msgBox(parent);
			msgBox.setWindowTitle(title);
			msgBox.setText(text);
			msgBox.setIcon(Icon::Critical);
			msgBox.setStandardButtons(buttons);
			msgBox.exec();
			return static_cast<StandardButton>(msgBox.result);
		}

		MessageBox::StandardButton MessageBox::question(QWidget* parent, const QString& title,
			const QString& text, int buttons)
		{
			MessageBox msgBox(parent);
			msgBox.setWindowTitle(title);
			msgBox.setText(text);
			msgBox.setIcon(Icon::Question);
			msgBox.setStandardButtons(buttons);
			msgBox.exec();
			return static_cast<StandardButton>(msgBox.result);
		}
	}

}

