#pragma once

#include <QPushButton>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class CarouselWidgetClass; };
QT_END_NAMESPACE

class CarouselWidget : public QWidget
{
	Q_OBJECT
private:
	std::map<int, QColor> colorMap;
public:
	CarouselWidget(QWidget* parent = nullptr);
	~CarouselWidget();
public:
	void setSize(size_t size);
private:
	size_t _size = 5;
private:
	void flaskGbox();
public:
	//0 空图像
	//1 绿色
	//2红色
	//其他随机
	void appendItem(int state);
	void dequeItem();
private:
	void setStandardItemStyle(QPushButton* pbtn, int i);
	void setStandardItemStyle012(QPushButton* pbtn, int i);

private:
	Ui::CarouselWidgetClass* ui;
};
