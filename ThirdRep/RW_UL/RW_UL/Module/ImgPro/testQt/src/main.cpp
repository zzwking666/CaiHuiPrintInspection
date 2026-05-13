#include <QApplication>

#include"ImageProcessTest.h"
#include"rqw_StorageSense.hpp"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	ImageProcessTest w;
	w.show();

	return a.exec();
}