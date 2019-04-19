#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("NB IoT Modem Configurator v1.0");
    w.setWindowFlags(Qt::Window
         | Qt::WindowCloseButtonHint
         | Qt::CustomizeWindowHint);
    w.show();

    return a.exec();
}
