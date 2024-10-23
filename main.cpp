#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWidget * mainWindow = new MainWidget();
    mainWindow->setWindowTitle("N bodies problem visualization");
    mainWindow->show();
    return app.exec();
}
