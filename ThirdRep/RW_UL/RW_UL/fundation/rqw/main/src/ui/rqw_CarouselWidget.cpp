#include "rqw_CarouselWidget.h"

#include "ui_CarouselWidget.h"

CarouselWidget::CarouselWidget(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::CarouselWidgetClass())
{
	ui->setupUi(this);
	flaskGbox();
}

CarouselWidget::~CarouselWidget()
{
	delete ui;
}

void CarouselWidget::setSize(size_t size)
{
	_size = size;
	flaskGbox();
}

void CarouselWidget::flaskGbox()
{
	QLayoutItem* item;
	while ((item = ui->gbox_CarouselWidgetBar->layout()->takeAt(0)) != nullptr) {
		delete item->widget();
		delete item;
	}

	// 添加 _size 个 QPushButton
	for (size_t i = 0; i < _size; ++i) {
		QPushButton* pbtn = new QPushButton(this);
		setStandardItemStyle(pbtn, 0);
		pbtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
		pbtn->setEnabled(false);
		ui->gbox_CarouselWidgetBar->layout()->addWidget(pbtn);
	}
}

void CarouselWidget::appendItem(int state)
{
	// 获取 gbox_CarouselWidgetBar 的布局
	QLayout* layout = ui->gbox_CarouselWidgetBar->layout();
	int count = layout->count();

	if (count > 0) {
		// 将所有按钮的样式表向右移动一位
		for (int i = count - 1; i > 0; --i) {
			QWidget* currentWidget = layout->itemAt(i)->widget();
			QWidget* previousWidget = layout->itemAt(i - 1)->widget();

			if (currentWidget && previousWidget) {
				QPushButton* currentButton = qobject_cast<QPushButton*>(currentWidget);
				QPushButton* previousButton = qobject_cast<QPushButton*>(previousWidget);

				if (currentButton && previousButton) {
					currentButton->setStyleSheet(previousButton->styleSheet());
				}
			}
		}

		// 设置最左边按钮的样式表
		QWidget* firstWidget = layout->itemAt(0)->widget();
		if (firstWidget) {
			QPushButton* firstButton = qobject_cast<QPushButton*>(firstWidget);
			if (firstButton) {
				setStandardItemStyle(firstButton, state);
			}
		}
	}
}

void CarouselWidget::dequeItem()
{
	// 获取 gbox_CarouselWidgetBar 的布局
	QLayout* layout = ui->gbox_CarouselWidgetBar->layout();
	int count = layout->count();

	if (count > 0) {
		// 将所有按钮的样式表向右移动一位
		for (int i = count - 1; i > 0; --i) {
			QWidget* currentWidget = layout->itemAt(i)->widget();
			QWidget* previousWidget = layout->itemAt(i - 1)->widget();

			if (currentWidget && previousWidget) {
				QPushButton* currentButton = qobject_cast<QPushButton*>(currentWidget);
				QPushButton* previousButton = qobject_cast<QPushButton*>(previousWidget);

				if (currentButton && previousButton) {
					currentButton->setStyleSheet(previousButton->styleSheet());
				}
			}
		}

		QWidget* firstWidget = layout->itemAt(0)->widget();
		if (firstWidget) {
			QPushButton* firstButton = qobject_cast<QPushButton*>(firstWidget);
			if (firstButton) {
				setStandardItemStyle(firstButton, 0);
			}
		}
	}
}

void CarouselWidget::setStandardItemStyle(QPushButton* pbtn, int i)
{
	if (i == 0 || i == 1 || i == 2)
	{
		setStandardItemStyle012(pbtn, i);
	}
	else
	{
		// 检查映射表中是否已有对应颜色
		if (colorMap.find(i) == colorMap.end())
		{
			// 如果没有，则生成随机颜色并存储
			int r = rand() % 256;
			int g = rand() % 256;
			int b = rand() % 256;
			colorMap[i] = QColor(r, g, b);
		}

		// 获取对应颜色
		QColor color = colorMap[i];

		QString styleSheet = QString(
			"QPushButton {"
			"    padding: 6px 14px;"
			"    border: 2px solid #CCC;"
			"    border-radius: 4px;"
			"    background-color: rgb(%1, %2, %3);"
			"    color: #444;"
			"    font-size: 15px;"
			"}"
			"QPushButton:hover {"
			"    border-color: #999;"
			"    background-color: #F5F5F5;"
			"}"
			"QPushButton:pressed {"
			"    border-color: #CCC;"
			"    background-color: rgb(%1, %2, %3);"
			"}"
			"QPushButton:released {"
			"    border-color: #CCC;"
			"    background-color: rgb(%1, %2, %3);"
			"}"
		).arg(color.red()).arg(color.green()).arg(color.blue());

		pbtn->setStyleSheet(styleSheet);
	}
}

void CarouselWidget::setStandardItemStyle012(QPushButton* pbtn, int i)
{
	switch (i)
	{
	case 0:
		pbtn->setStyleSheet(
			"QPushButton {"
			"    padding: 6px 14px;"
			"    border: 2px solid #CCC;"
			"    border-radius: 4px;"
			"    background-color: rgb(255, 255, 255);"
			"    color: #444;"
			"    font-size: 15px;"
			"}"
			"QPushButton:hover {"
			"    border-color: #999;"
			"    background-color: #F5F5F5;"
			"}"
			"QPushButton:pressed {"
			"    border-color: #CCC;"
			"    background-color: rgb(255, 255, 255);"
			"}"
			"QPushButton:released {"
			"    border-color: #CCC;"
			"    background-color: rgb(255, 255, 255);"
			"}"
		);
		break;
	case 1:
		pbtn->setStyleSheet(
			"QPushButton {"
			"    padding: 6px 14px;"
			"    border: 2px solid #CCC;"
			"    border-radius: 4px;"
			"    background-color: rgb(130, 255, 34);"
			"    color: #444;"
			"    font-size: 15px;"
			"}"
			"QPushButton:hover {"
			"    border-color: #999;"
			"    background-color: #F5F5F5;"
			"}"
			"QPushButton:pressed {"
			"    border-color: #CCC;"
			"    background-color: rgb(130, 255, 34);"
			"}"
			"QPushButton:released {"
			"    border-color: #CCC;"
			"    background-color: rgb(130, 255, 34);"
			"}"
		);
		break;
	case 2:
		pbtn->setStyleSheet(
			"QPushButton {"
			"    padding: 6px 14px;"
			"    border: 2px solid #CCC;"
			"    border-radius: 4px;"
			"    background-color: rgb(255, 68, 11);"
			"    color: #444;"
			"    font-size: 15px;"
			"}"
			"QPushButton:hover {"
			"    border-color: #999;"
			"    background-color: #F5F5F5;"
			"}"
			"QPushButton:pressed {"
			"    border-color: #CCC;"
			"    background-color: rgb(255, 68, 11);"
			"}"
			"QPushButton:released {"
			"    border-color: #CCC;"
			"    background-color: rgb(255, 68, 11);"
			"}"
		);
		break;
	}
}