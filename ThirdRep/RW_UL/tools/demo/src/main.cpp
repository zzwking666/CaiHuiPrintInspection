#include <iostream>
#include <QPainter>
#include <random>
#include <QtWidgets/QApplication>
#include "FullKeyBoard.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	FullKeyBoard keyboard;
	keyboard.exec();

	QString inputValue = keyboard.getValue();
	std::cout << "输入内容: " << inputValue.toStdString() << std::endl;


	return a.exec();
}