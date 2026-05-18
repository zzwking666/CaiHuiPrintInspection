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
	Modules::getInstance().uiModule._caiHuiPrintInspection = &w;
	Modules::getInstance().connect();
	Modules::getInstance().start();

	w.setFixedSize(1920, 1080);
#ifdef NDEBUG
	//w.showFullScreen();
#else
	//w.show();
	//w.showFullScreen();
#endif

	// 获取所有屏幕
	QList<QScreen*> screens = QGuiApplication::screens();
	if (screens.size() >= 2) {
		// 程序放到副屏 (1)
		QRect screen = screens[1]->geometry();
		w.move(screen.x(), screen.y());
		w.showFullScreen();
	}
	else
	{
		w.showFullScreen();
	}

    return a.exec();
}
