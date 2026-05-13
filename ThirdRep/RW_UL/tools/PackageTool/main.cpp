#include "PackageTool.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PackageTool window;
    window.show();
    return app.exec();
}
