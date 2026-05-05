#include <QMessageBox>
#include <QtWidgets/QApplication>

#include "Modules.hpp"
#include "CaiHuiPrintInspection.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

	if (!Modules::check())
	{
		return 1;
	}

	Modules::getInstance().build();

	CaiHuiPrintInspection w;
	Modules::getInstance().uiModule._maiLiDingZi = &w;
	Modules::getInstance().connect();
	Modules::getInstance().start();

	w.setFixedSize(1920, 1080);
#ifdef NDEBUG
	w.showFullScreen();
#else
	//w.show();
	w.showFullScreen();
#endif
    return a.exec();
}
