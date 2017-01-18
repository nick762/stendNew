#include "mainwindow.h"
#include <QApplication>
#include <QtCore/QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("CP-1251")); //изменения
    QApplication a(argc, argv);
    MainWindow w;
    w.setStyleSheet("QMainWondow {background-color: #8FBC8F;}");
    w.show();

    return a.exec();
}
